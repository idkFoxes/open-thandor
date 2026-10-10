/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/campaign_map_control.cpp
 * Project code (not in the original game)
 */

/* The mission map control of the campaign tab and the check mark of a finished campaign on its button
   (docs/plans/campaign_map.md 5.5, 6.2-6.4; include/thandor/ui/frontend/campaign_map_control.h). Measures and
   colours follow the approved mockup v4 (ot-scratch campaign-mockup tools/compose.py page_v3). All drawing goes
   through the framebuffer slots (texture blits of win.gfx, g_GraphicsFramebufferFillRectArgb, the rich-text
   line), in logical pixels: the GPU renderers record and scale it like every other control. */

#include <thandor/ui/frontend/campaign_map_control.h>
#include <algorithm>
#include <array>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace {

/* Colours (ARGB) of mockup v4. */
constexpr PackedArgb32 CAMPAIGN_MAP_DONE_FILL_ARGB = 0xCC466846;     /* GREEN 70,104,70 at 80 % */
constexpr PackedArgb32 CAMPAIGN_MAP_SELECTED_FILL_ARGB = 0xFF484848; /* SEL 72: the pressed node's face */
constexpr PackedArgb32 CAMPAIGN_MAP_PATH_LIT_ARGB = 0x8CAEAEAE;      /* TEXT 174 at 55 % */
constexpr PackedArgb32 CAMPAIGN_MAP_PATH_DIM_ARGB = 0x29AEAEAE;      /* TEXT 174 at 16 % */
constexpr PackedArgb32 CAMPAIGN_MAP_LEGEND_LINE_ARGB = 0x8CAEAEAE;   /* the legend's double line, 55 % */

/* Legend (mockup page_v3): the text ends 2 pixels left of the frame's last column, its top 17 pixels above the
   frame's last row; the double line runs from 20 to 6 pixels left of the text, 9 pixels above the last row, at
   -1 and +1. */
constexpr int32_t CAMPAIGN_MAP_LEGEND_TEXT_RIGHT_GAP = 2;
constexpr int32_t CAMPAIGN_MAP_LEGEND_TEXT_ABOVE_LAST_ROW = 17;
constexpr int32_t CAMPAIGN_MAP_LEGEND_LINE_START = 20;
constexpr int32_t CAMPAIGN_MAP_LEGEND_LINE_END = 6;
constexpr int32_t CAMPAIGN_MAP_LEGEND_LINE_ABOVE_LAST_ROW = 9;

/* A carry edge: two parallel lines at -1 and +1 pixels. */
constexpr std::array<int32_t, 2> CAMPAIGN_MAP_CARRY_OFFSETS = {-1, 1};
constexpr std::array<int32_t, 1> CAMPAIGN_MAP_SINGLE_OFFSET = {0};

/* Number of map controls (FrontendCampaignMapControl.mapIndex): the frontend image has one. */
constexpr uint32_t CAMPAIGN_MAP_SLOT_COUNT = 1;

/* The check mark (5.5, Q11): seven columns of a two-pixel stroke, the short stroke down to the right, then the long
   one up; COLUMN_TOP is the first row of each column, the mark is CHECK_MARK_HEIGHT rows high. It sits
   CHECK_MARK_GAP pixels right of the label, its last row CHECK_MARK_BASELINE_LIFT rows above the label's
   baseline. */
constexpr std::array<int32_t, 7> CHECK_MARK_COLUMN_TOP = {2, 3, 4, 3, 2, 1, 0};
constexpr int32_t CHECK_MARK_STROKE = 2;
constexpr int32_t CHECK_MARK_WIDTH = static_cast<int32_t>(CHECK_MARK_COLUMN_TOP.size());
constexpr int32_t CHECK_MARK_HEIGHT = 4 + CHECK_MARK_STROKE;
constexpr int32_t CHECK_MARK_GAP = 3;
constexpr int32_t CHECK_MARK_BASELINE_LIFT = 1;
/* Room between the button frame and the label when the label is cut for the mark. */
constexpr int32_t CHECK_MARK_BUTTON_PADDING = 4;

struct CampaignMapState {
    const CampaignGraph *graph = nullptr;
    const CampaignMapLayout *layout = nullptr;
    std::vector<CampaignMapNodeState> nodeStates;
    int selectedNode = -1;
    FrontendCampaignMapCallback callback = nullptr;
};

std::array<CampaignMapState, CAMPAIGN_MAP_SLOT_COUNT> g_CampaignMapStates;

CampaignMapState *CampaignMap_State(const FrontendCampaignMapControl *control)
{
    if (control == nullptr || control->mapIndex >= CAMPAIGN_MAP_SLOT_COUNT) {
        return nullptr;
    }
    return &g_CampaignMapStates[control->mapIndex];
}

int CampaignMap_NodeCount(const CampaignMapState &state)
{
    return state.graph != nullptr ? static_cast<int>(state.graph->nodes.size()) : 0;
}

CampaignMapNodeState CampaignMap_NodeState(const CampaignMapState &state, int node)
{
    if (node < 0 || static_cast<size_t>(node) >= state.nodeStates.size()) {
        return CAMPAIGN_MAP_NODE_LOCKED;
    }
    return state.nodeStates[static_cast<size_t>(node)];
}

/* Index of the node whose box contains the frame-relative point, -1 for none. */
int CampaignMap_NodeAt(const CampaignMapState &state, int32_t x, int32_t y)
{
    int count = CampaignMap_NodeCount(state);
    for (int i = 0; i < count; i++) {
        const CampaignMapNodeLayout &node = state.layout->nodes[static_cast<size_t>(i)];
        int32_t left = node.x - CAMPAIGN_MAP_NODE_WIDTH / 2;
        int32_t top = node.y - CAMPAIGN_MAP_NODE_HEIGHT / 2;
        if (x >= left && x < left + CAMPAIGN_MAP_NODE_WIDTH && y >= top && y < top + CAMPAIGN_MAP_NODE_HEIGHT) {
            return i;
        }
    }
    return -1;
}

UiPixelRect UiPixelRect_Intersect(UiPixelRect a, UiPixelRect b)
{
    return UiPixelRect{std::max(a.left, b.left), std::max(a.top, b.top), std::min(a.right, b.right),
                       std::min(a.bottom, b.bottom)};
}

void FillRect(UiPixelRect clip, int32_t left, int32_t top, int32_t right, int32_t bottom, PackedArgb32 argb)
{
    g_GraphicsFramebufferFillRectArgb(clip.bottom, clip.right, clip.top, clip.left, bottom, right, top, left, argb,
                                      g_FramebufferAccess);
}

/* One edge polyline (points frame-relative, offset by originX/originY). Each segment covers its first point up to
   its last; every segment but the last stops one pixel short of its end, so a translucent corner pixel is not
   blended twice. */
void CampaignMap_DrawEdge(const CampaignMapEdgeLayout &edge, bool carry, PackedArgb32 argb, int32_t originX,
                          int32_t originY, UiPixelRect clip)
{
    std::span<const int32_t> offsets = carry ? std::span<const int32_t>(CAMPAIGN_MAP_CARRY_OFFSETS)
                                             : std::span<const int32_t>(CAMPAIGN_MAP_SINGLE_OFFSET);
    int32_t count = std::min(edge.pointCount, CAMPAIGN_MAP_EDGE_MAX_POINTS);
    for (int32_t i = 0; i + 1 < count; i++) {
        CampaignMapPoint a = edge.points[i];
        CampaignMapPoint b = edge.points[i + 1];
        bool last = i + 2 == count;
        if (a == b) {
            continue; /* empty: its pixel belongs to the next segment */
        }
        if (a.y == b.y) {
            int32_t endX = last ? b.x : b.x + (b.x > a.x ? -1 : 1);
            int32_t left = std::min(a.x, endX);
            int32_t right = std::max(a.x, endX);
            for (int32_t offset : offsets) {
                FillRect(clip, originX + left, originY + a.y + offset, originX + right + 1,
                         originY + a.y + offset + 1, argb);
            }
        } else if (a.x == b.x) {
            int32_t endY = last ? b.y : b.y + (b.y > a.y ? -1 : 1);
            int32_t top = std::min(a.y, endY);
            int32_t bottom = std::max(a.y, endY);
            for (int32_t offset : offsets) {
                FillRect(clip, originX + a.x + offset, originY + top, originX + a.x + offset + 1,
                         originY + bottom + 1, argb);
            }
        }
        /* the layout only makes axis-aligned segments; anything else is not drawn */
    }
}

/* Writes the decimal number into buffer (at least 12 code units) as a rich-text stream (plain glyph code units,
   terminated). */
void CampaignMap_FormatNumber(int32_t value, std::span<uint16_t, 12> buffer)
{
    std::array<uint16_t, 12> digits{};
    size_t count = 0;
    bool negative = value < 0;
    uint32_t magnitude = negative ? 0u - static_cast<uint32_t>(value) : static_cast<uint32_t>(value);
    do {
        digits[count++] = static_cast<uint16_t>(u'0' + magnitude % 10u);
        magnitude /= 10u;
    } while (magnitude != 0 && count < digits.size());
    size_t out = 0;
    if (negative && out + 1 < buffer.size()) {
        buffer[out++] = u'-';
    }
    while (count != 0 && out + 1 < buffer.size()) {
        buffer[out++] = digits[--count];
    }
    buffer[out] = 0;
}

void CampaignMap_DrawNode(const CampaignMapState &state, int node, int32_t originX, int32_t originY,
                          UiPixelRect clip)
{
    const CampaignMapNodeLayout &nodeLayout = state.layout->nodes[static_cast<size_t>(node)];
    CampaignMapNodeState nodeState = CampaignMap_NodeState(state, node);
    bool selected = node == state.selectedNode;
    UiPixelRect box;
    box.left = originX + nodeLayout.x - CAMPAIGN_MAP_NODE_WIDTH / 2;
    box.top = originY + nodeLayout.y - CAMPAIGN_MAP_NODE_HEIGHT / 2;
    box.right = box.left + CAMPAIGN_MAP_NODE_WIDTH;
    box.bottom = box.top + CAMPAIGN_MAP_NODE_HEIGHT;

    GraphicsSubresourceIndex frame = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    UiPackedTextStyle textStyle = g_UiTextStyleNormal;
    if (nodeState == CAMPAIGN_MAP_NODE_DONE) {
        FillRect(clip, box.left + 1, box.top + 1, box.right - 1, box.bottom - 1, CAMPAIGN_MAP_DONE_FILL_ARGB);
        textStyle = g_UiTextStyleSelected;
    } else if (selected) {
        FillRect(clip, box.left + 1, box.top + 1, box.right - 1, box.bottom - 1, CAMPAIGN_MAP_SELECTED_FILL_ARGB);
    }
    if (selected) {
        frame = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME + UI_WINDOW_FRAME_PIECE_COUNT;
        textStyle = g_UiTextStyleSelected;
    } else if (nodeState == CAMPAIGN_MAP_NODE_LOCKED) {
        frame = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    if (nodeState == CAMPAIGN_MAP_NODE_LOCKED) {
        textStyle = g_UiTextStyleDisabled;
    }
    UiFramedFrame_Draw(box, frame, clip);

    std::array<uint16_t, 12> label{};
    CampaignMap_FormatNumber(state.graph->nodes[static_cast<size_t>(node)].missionNumber, label);
    RichTextExtent extent = RichTextCommandStream_MeasureLine(textStyle, label.data());
    int32_t textX = box.left + ((CAMPAIGN_MAP_NODE_WIDTH - static_cast<int32_t>(extent.widthPixels)) >> 1);
    int32_t textY = box.top + ((CAMPAIGN_MAP_NODE_HEIGHT - static_cast<int32_t>(extent.heightPixels)) >> 1);
    RichTextCommandStream_DrawSingleLine(clip.bottom, clip.right, clip.top, clip.left, textStyle, label.data(),
                                         textY, textX);
}

void CampaignMap_DrawLegend(UiPixelRect frameRect, UiPixelRect clip)
{
    uint16_t *text = TextResource_Resolve(TEXT_ID_CAMPAIGN_LEGEND_CARRY);
    RichTextExtent extent = RichTextCommandStream_MeasureLine(g_UiTextStyleNormal, text);
    int32_t lastColumn = frameRect.right - 1;
    int32_t lastRow = frameRect.bottom - 1;
    int32_t textX = lastColumn - CAMPAIGN_MAP_LEGEND_TEXT_RIGHT_GAP - static_cast<int32_t>(extent.widthPixels);
    int32_t lineY = lastRow - CAMPAIGN_MAP_LEGEND_LINE_ABOVE_LAST_ROW;
    for (int32_t offset : CAMPAIGN_MAP_CARRY_OFFSETS) {
        FillRect(clip, textX - CAMPAIGN_MAP_LEGEND_LINE_START, lineY + offset, textX - CAMPAIGN_MAP_LEGEND_LINE_END + 1,
                 lineY + offset + 1, CAMPAIGN_MAP_LEGEND_LINE_ARGB);
    }
    RichTextCommandStream_DrawSingleLine(clip.bottom, clip.right, clip.top, clip.left, g_UiTextStyleNormal, text,
                                         lastRow - CAMPAIGN_MAP_LEGEND_TEXT_ABOVE_LAST_ROW, textX);
}

/* The text shadow offset of a style's palette entry, as the rich-text renderer uses it
   (g_RichTextShadowOffsetPalette: entries 0, 1, 3 and 7 two pixels, entry 2 one, the others none). */
int32_t CheckMark_ShadowOffset(uint32_t paletteIndex)
{
    switch (paletteIndex) {
    case 0:
    case 1:
    case 3:
    case 7:
        return 2;
    case 2:
        return 1;
    default:
        return 0;
    }
}

/* The check mark with its top-left pixel at (x, top), in the label colour of textStyle with the text's shadow. */
void CheckMark_Draw(UiPixelRect clip, int32_t x, int32_t top, UiPackedTextStyle textStyle)
{
    uint32_t paletteIndex = textStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
    PackedArgb32 colour = g_RichTextColorPaletteArgb[paletteIndex];
    int32_t shadow = CheckMark_ShadowOffset(paletteIndex);
    if (shadow != 0) {
        for (int32_t column = 0; column < CHECK_MARK_WIDTH; column++) {
            int32_t columnTop = top + CHECK_MARK_COLUMN_TOP[static_cast<size_t>(column)] + shadow;
            FillRect(clip, x + column + shadow, columnTop, x + column + shadow + 1, columnTop + CHECK_MARK_STROKE,
                     TEXT_SHADOW_COLOR_ARGB);
        }
    }
    for (int32_t column = 0; column < CHECK_MARK_WIDTH; column++) {
        int32_t columnTop = top + CHECK_MARK_COLUMN_TOP[static_cast<size_t>(column)];
        FillRect(clip, x + column, columnTop, x + column + 1, columnTop + CHECK_MARK_STROKE, colour);
    }
}

} // namespace

void FrontendCampaignMapControl_SetGraph(FrontendCampaignMapControl *control, const CampaignGraph *graph,
                                         const CampaignMapLayout *layout,
                                         std::span<const CampaignMapNodeState> nodeStates)
{
    CampaignMapState *state = CampaignMap_State(control);
    if (state == nullptr) {
        return;
    }
    bool layoutMatches = graph != nullptr && layout != nullptr && layout->nodes.size() == graph->nodes.size() &&
                         layout->edges.size() == graph->edges.size();
    state->graph = layoutMatches ? graph : nullptr;
    state->layout = layoutMatches ? layout : nullptr;
    state->nodeStates.assign(nodeStates.begin(), nodeStates.end());
    if (state->selectedNode >= CampaignMap_NodeCount(*state)) {
        state->selectedNode = -1;
    }
    UiNode_InvalidateRoot(UiNode_As<UiNodeBase>(control));
}

void FrontendCampaignMapControl_SetSelectedNode(FrontendCampaignMapControl *control, int nodeIndex)
{
    CampaignMapState *state = CampaignMap_State(control);
    if (state == nullptr) {
        return;
    }
    int selected = nodeIndex >= 0 && nodeIndex < CampaignMap_NodeCount(*state) ? nodeIndex : -1;
    if (selected != state->selectedNode) {
        state->selectedNode = selected;
        UiNode_InvalidateRoot(UiNode_As<UiNodeBase>(control));
    }
}

int FrontendCampaignMapControl_GetSelectedNode(const FrontendCampaignMapControl *control)
{
    const CampaignMapState *state = CampaignMap_State(control);
    return state != nullptr ? state->selectedNode : -1;
}

void FrontendCampaignMapControl_SetCallback(FrontendCampaignMapControl *control, FrontendCampaignMapCallback callback)
{
    CampaignMapState *state = CampaignMap_State(control);
    if (state != nullptr) {
        state->callback = callback;
    }
}

/* drawClipped of g_FrontendCampaignMapControlVtable: the sunken frame of the old campaign list (frame style A of
   win.gfx, as campaignsScroller draws it), then inside it the edges, the nodes and the legend (6.3), clipped to
   the frame's interior. A suppressed map draws nothing. */
void FrontendCampaignMapControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FrontendCampaignMapControl *control)
{
    const UiNodeBase &base = control->panel.base;
    if (Any(base.nodeFlags & UI_NODE_SUPPRESSED)) {
        return;
    }
    if (g_GraphicsFramebufferBeginAccess()) {
        return;
    }
    UiPixelRect frameRect{base.left, base.top, base.left + base.layoutWidth, base.top + base.layoutHeight};
    UiPixelRect clip = UiPixelRect_Intersect(UiPixelRect{clipLeft, clipTop, clipRight, clipBottom}, frameRect);
    if (clip.left < clip.right && clip.top < clip.bottom) {
        UiFramedFrame_Draw(frameRect, UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST, clip);
        GraphicsTextureLogicalSize corner =
            g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST, g_UiWindowTextureSource);
        UiPixelRect interior{frameRect.left + static_cast<int32_t>(corner.logicalWidthPixels),
                             frameRect.top + static_cast<int32_t>(corner.logicalHeightPixels),
                             frameRect.right - static_cast<int32_t>(corner.logicalWidthPixels),
                             frameRect.bottom - static_cast<int32_t>(corner.logicalHeightPixels)};
        UiPixelRect inner = UiPixelRect_Intersect(clip, interior);
        const CampaignMapState *state = CampaignMap_State(control);
        if (state != nullptr && state->graph != nullptr && inner.left < inner.right && inner.top < inner.bottom) {
            const CampaignGraph &graph = *state->graph;
            const CampaignMapLayout &layout = *state->layout;
            /* edges first, under the nodes: lit when the source is done and the target is not locked */
            for (size_t e = 0; e < graph.edges.size(); e++) {
                const CampaignGraphEdge &edge = graph.edges[e];
                bool lit = CampaignMap_NodeState(*state, edge.from) == CAMPAIGN_MAP_NODE_DONE &&
                           CampaignMap_NodeState(*state, edge.to) != CAMPAIGN_MAP_NODE_LOCKED;
                CampaignMap_DrawEdge(layout.edges[e], edge.carryOver,
                                     lit ? CAMPAIGN_MAP_PATH_LIT_ARGB : CAMPAIGN_MAP_PATH_DIM_ARGB, frameRect.left,
                                     frameRect.top, inner);
            }
            for (int node = 0; node < CampaignMap_NodeCount(*state); node++) {
                CampaignMap_DrawNode(*state, node, frameRect.left, frameRect.top, inner);
            }
            if (graph.hasCarryEdge) {
                CampaignMap_DrawLegend(frameRect, inner);
            }
        }
    }
    g_GraphicsFramebufferEndAccess();
}

/* nonRightPress of g_FrontendCampaignMapControlVtable: a press on a node selects it and reports it
   (startRequested false); a double click (UI_NODE_REPEAT_OR_DOUBLE_CLICK, as the list's confirm) on the selected
   node that is not locked requests its start. A single press on the selected node and a press beside the nodes
   do nothing. */
void FrontendCampaignMapControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendCampaignMapControl *control)
{
    (void)wheelDelta;
    const UiNodeBase &base = control->panel.base;
    CampaignMapState *state = CampaignMap_State(control);
    if (Any(base.nodeFlags & UI_NODE_SUPPRESSED) || state == nullptr || state->graph == nullptr) {
        return;
    }
    int node = CampaignMap_NodeAt(*state, pointerX - base.left, pointerY - base.top);
    if (node < 0) {
        return;
    }
    if (node == state->selectedNode) {
        if (Any(base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) &&
            CampaignMap_NodeState(*state, node) != CAMPAIGN_MAP_NODE_LOCKED && state->callback != nullptr) {
            state->callback(node, true);
        }
        return;
    }
    state->selectedNode = node;
    UiNode_InvalidateRoot(UiNode_As<UiNodeBase>(control));
    if (state->callback != nullptr) {
        state->callback(node, false);
    }
}

/* hitTest of g_FrontendCampaignMapControlVtable: the map itself anywhere inside its box unless it is suppressed. */
UiNodeBase *FrontendCampaignMapControl_HitTest
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,FrontendCampaignMapControl *control)
{
    UiNodeBase &base = control->panel.base;
    if (Any(base.nodeFlags & UI_NODE_SUPPRESSED) || pointerX < base.left || pointerY < base.top ||
        pointerX - base.left >= base.layoutWidth || pointerY - base.top >= base.layoutHeight) {
        return UI_NODE_NONE;
    }
    return &base;
}

void UiFramedTextButton_SetCheckMark(UiFramedTextButtonControl *button, bool on)
{
    if (button == nullptr) {
        return;
    }
    UiNodeVtable *vtable = on ? &g_FrontendCampaignCheckButtonVtable : &g_UiFramedTextButtonControlVtable;
    UiNodeBase &base = button->selectable.base;
    if (base.vtable.get() != vtable) {
        base.vtable = vtable;
        UiNode_InvalidateRoot(&base);
    }
}

/* drawClipped of g_FrontendCampaignCheckButtonVtable: a suppressed button as the framed text button draws it;
   otherwise its frame (normal or selected, plain or inset) and its label in the state's style (the button's own
   font and palette bytes as UiFramedTextButtonControl_DrawClipped applies them) followed by the check mark,
   centred together; a label too wide for both is cut at the right so the mark always fits. No keyboard focus
   mark (the campaign buttons are mouse only, Q9). Then the children. */
void UiFramedTextButtonControl_DrawClippedWithCheckMark
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFramedTextButtonControl *control)
{
    UiSelectableControl &selectable = control->selectable;
    if (Any(selectable.base.nodeFlags & UI_NODE_SUPPRESSED)) {
        UiFramedTextButtonControl_DrawClipped(clipBottom, clipRight, clipTop, clipLeft, control);
        return;
    }
    if (!g_GraphicsFramebufferBeginAccess()) {
        bool selected = Any(selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED);
        GraphicsSubresourceIndex frame = Any(selectable.stateFlags & UI_BUTTON_FRAME_INSET)
                                             ? UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME
                                             : UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
        if (selected) {
            frame += UI_WINDOW_FRAME_PIECE_COUNT;
        }
        int32_t left = selectable.base.left;
        int32_t top = selectable.base.top;
        int32_t width = selectable.base.layoutWidth;
        int32_t height = selectable.base.layoutHeight;
        UiPixelRect clip{clipLeft, clipTop, clipRight, clipBottom};
        UiFramedFrame_Draw(UiPixelRect{left, top, left + width, top + height}, frame, clip);

        UiPackedTextStyle textStyle = selected ? g_UiTextStyleSelected : g_UiTextStyleNormal;
        if (Any(selectable.stateFlags & UI_BUTTON_OWN_STYLE_FONT)) {
            textStyle = (textStyle & ~UI_TEXT_STYLE_FONT_BYTE) | (control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE);
        }
        if (Any(selectable.stateFlags & UI_BUTTON_OWN_STYLE_PALETTE)) {
            textStyle = (textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE) |
                        (control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE);
        }
        uint16_t *text = TextResource_Resolve(control->textResourceId);
        RichTextExtent extent = RichTextCommandStream_MeasureLine(textStyle, text);
        int32_t textWidth = static_cast<int32_t>(extent.widthPixels);
        int32_t textHeight = static_cast<int32_t>(extent.heightPixels);
        int32_t available = width - 2 * CHECK_MARK_BUTTON_PADDING;
        int32_t groupWidth = textWidth + CHECK_MARK_GAP + CHECK_MARK_WIDTH;
        int32_t textX;
        int32_t textRight;
        if (groupWidth <= available) {
            textX = left + ((width - groupWidth) >> 1);
            textRight = textX + textWidth;
        } else {
            textX = left + CHECK_MARK_BUTTON_PADDING;
            textRight = std::max(textX, textX + available - CHECK_MARK_GAP - CHECK_MARK_WIDTH);
        }
        int32_t textY = top + ((height - textHeight) >> 1);
        RichTextCommandStream_DrawSingleLine(clipBottom, std::min(clipRight, textRight), clipTop, clipLeft, textStyle,
                                             text, textY, textX);
        int32_t markBottom = textY + textHeight - 1 - CHECK_MARK_BASELINE_LIFT;
        CheckMark_Draw(clip, textRight + CHECK_MARK_GAP, markBottom - CHECK_MARK_HEIGHT + 1, textStyle);
        g_GraphicsFramebufferEndAccess();
    }
    UiContainer_DrawIntersectingChildren(clipBottom, clipRight, clipTop, clipLeft, UiNode_As<UiNodeBase>(control));
}

/* The map: its own draw, press and hit test; the other slots are the image panel's (no hover in v1, no keyboard
   control, Q9). */
UiNodeVtable g_FrontendCampaignMapControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(FrontendCampaignMapControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(FrontendCampaignMapControl_NonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(FrontendCampaignMapControl_HitTest),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

/* A campaign button of a finished campaign: g_UiFramedTextButtonControlVtable with the check mark draw. */
UiNodeVtable g_FrontendCampaignCheckButtonVtable = {
        .relocate = UI_SLOT(UiFramedTextButtonControl_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiFramedTextButtonControl_DrawClippedWithCheckMark),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiFramedTextButtonControl_NonRightPress),
        .nonRightRelease = UI_SLOT(UiFramedTextButtonControl_NonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiFramedTextButtonControl_NonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiFramedTextButtonControl_HitTestRect),
        .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};
