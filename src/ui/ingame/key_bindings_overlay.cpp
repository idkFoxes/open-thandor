/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/key_bindings_overlay.cpp
 * Project code (not in the original game)
 */

/* The in-game host of the key binding panel (include/thandor/ui/ingame/key_bindings_overlay.h).

   Nodes: the layer (module storage, g_InGameKeyBindingsLayerVtable), linked as the last child of the in-game root
   panel while the overlay is open; the key binding panel is attached to it. The layer covers the whole root and is
   hit wherever the panel is not, so presses, right presses and wheel turns beside the panel go nowhere. */

#include <thandor/ui/ingame/key_bindings_overlay.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_bindings_panel.h>

#include <algorithm>
#include <cstring>

/* The panel's size: the frontend page's panel (608 x 284) plus the background inset (12 on every side) and the
   title line (26), so the list, status line and buttons get the same room as on the frontend page. */
static constexpr int32_t INGAME_KEY_BINDINGS_OVERLAY_WIDTH = 608 + 2 * 12;
static constexpr int32_t INGAME_KEY_BINDINGS_OVERLAY_HEIGHT = 284 + 2 * 12 + 26;
static constexpr int32_t INGAME_KEY_BINDINGS_OVERLAY_MARGIN = 8; /* to the world view's edges when it is smaller */
static constexpr UiAnchorFractionQ31 INGAME_KEY_BINDINGS_OVERLAY_ANCHOR_FULL = 0x80000000;

extern UiNodeVtable g_InGameKeyBindingsLayerVtable;

/* Module data. */

static UiPanelControl s_InGameKeyBindingsLayer;
static bool s_InGameKeyBindingsOverlayOpen = false;
static UiNodeBase *s_InGameKeyBindingsOverlayPreviousFocus = UI_NODE_NONE; /* restored on close */

/* The layer's handlers: nothing beside the panel reacts. */
static void InGameKeyBindingsLayer_IgnorePress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *node)
{
  (void)wheelDelta;
  (void)pointerY;
  (void)pointerX;
  (void)node;
}

static void InGameKeyBindingsLayer_Relocate(UiSerializedRelocationDelta relocationDelta,UiNodeBase *node)
{
  /* module node linked at run time: nothing to relocate */
  (void)relocationDelta;
  (void)node;
}

/* The panel's close callback ("Zurueck", Esc). */
static void InGameKeyBindingsOverlay_OnPanelClose(void *context)
{
  (void)context;
  InGameKeyBindingsOverlay_Close();
}

void InGameKeyBindingsOverlay_Open(UiNodeBase *sourceNode)
{
  UiNodeBase *root = sourceNode;
  UiNodeBase *layer = &s_InGameKeyBindingsLayer.root.base;

  while (root->parent != UI_NODE_NONE) {
    root = root->parent;
  }
  InGameKeyBindingsOverlay_Close();
  s_InGameKeyBindingsOverlayPreviousFocus = g_UiKeyboardFocusNode;

  /* the layer: the root's box, the last child of the root */
  std::memset(static_cast<void *>(&s_InGameKeyBindingsLayer),0,sizeof(s_InGameKeyBindingsLayer));
  layer->vtable = &g_InGameKeyBindingsLayerVtable;
  layer->parent = root;
  layer->nextSibling = UI_NODE_NONE;
  layer->firstChild = UI_NODE_NONE;
  layer->rightAnchorQ31 = INGAME_KEY_BINDINGS_OVERLAY_ANCHOR_FULL;
  layer->bottomAnchorQ31 = INGAME_KEY_BINDINGS_OVERLAY_ANCHOR_FULL;
  layer->left = root->left;
  layer->top = root->top;
  layer->right = root->right;
  layer->bottom = root->bottom;
  layer->layoutWidth = layer->right - layer->left;
  layer->layoutHeight = layer->bottom - layer->top;
  if (root->firstChild == UI_NODE_NONE) {
    root->firstChild = layer;
  }
  else {
    UiNodeBase *last = root->firstChild;
    while (last->nextSibling != UI_NODE_NONE) {
      last = last->nextSibling;
    }
    last->nextSibling = layer;
  }
  s_InGameKeyBindingsOverlayOpen = true;

  /* the panel: centred over the world view (left of the side panel), the frontend size when it fits */
  const UiNodeBase &worldArea = InGameUi_Image(root)->worldViewArea.base;
  const int32_t areaWidth = worldArea.right - worldArea.left;
  const int32_t areaHeight = worldArea.bottom - worldArea.top;
  const int32_t width = std::max(KEY_BINDINGS_PANEL_MIN_WIDTH,
                                 std::min(INGAME_KEY_BINDINGS_OVERLAY_WIDTH,
                                          areaWidth - 2 * INGAME_KEY_BINDINGS_OVERLAY_MARGIN));
  const int32_t height = std::max(KEY_BINDINGS_PANEL_MIN_HEIGHT,
                                  std::min(INGAME_KEY_BINDINGS_OVERLAY_HEIGHT,
                                           areaHeight - 2 * INGAME_KEY_BINDINGS_OVERLAY_MARGIN));
  /* KEY_BINDINGS_PANEL_CENTERED: relative to the layer's (the root's) centre */
  const int32_t centerX = (worldArea.left + worldArea.right) / 2 - (layer->left + layer->right) / 2;
  const int32_t centerY = (worldArea.top + worldArea.bottom) / 2 - (layer->top + layer->bottom) / 2;
  const int32_t left = centerX - width / 2;
  const int32_t top = centerY - height / 2;
  KeyBindingsPanel_Attach(layer,UiPixelRect{left,top,left + width,top + height},
                          KEY_BINDINGS_PANEL_CENTERED | KEY_BINDINGS_PANEL_BACKGROUND | KEY_BINDINGS_PANEL_TITLE,
                          &InGameKeyBindingsOverlay_OnPanelClose,nullptr);
  Thandor_Log("in-game key bindings overlay: open");
  UiNode_InvalidateRoot(root);
}

/* True when node lies in the layer's subtree. */
static bool InGameKeyBindingsOverlay_Contains(UiNodeBase *node)
{
  for (; node != nullptr && node != UI_NODE_NONE; node = node->parent) {
    if (node == &s_InGameKeyBindingsLayer.root.base) {
      return true;
    }
  }
  return false;
}

void InGameKeyBindingsOverlay_Close()
{
  UiNodeBase *layer = &s_InGameKeyBindingsLayer.root.base;
  UiNodeBase *root;

  if (!s_InGameKeyBindingsOverlayOpen) {
    return;
  }
  if (KeyBindingsPanel_IsAttached() && KeyBindingsPanel_Root()->parent == layer) {
    KeyBindingsPanel_Detach();
  }
  /* pointer capture and tooltip target on the layer itself: once unlinked its root chain ends at the layer,
     so neither UiRootStack_Pop nor the in-game teardown would recognise them as belonging to the closing root */
  if (InGameKeyBindingsOverlay_Contains(g_UiPointerCaptureTarget)) {
    g_UiPointerCaptureTarget = UI_NODE_NONE;
    g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  }
  if (InGameKeyBindingsOverlay_Contains(g_UiTooltipState.targetNode)) {
    g_UiTooltipState.targetNode = nullptr;
  }
  if (g_UiImageControlHoverTarget != nullptr &&
      InGameKeyBindingsOverlay_Contains(&g_UiImageControlHoverTarget->selectable.base)) {
    g_UiImageControlHoverTarget = nullptr;
  }
  root = layer->parent;
  if (root->firstChild == layer) {
    root->firstChild = layer->nextSibling;
  }
  else {
    for (UiNodeBase *node = root->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
      if (node->nextSibling == layer) {
        node->nextSibling = layer->nextSibling;
        break;
      }
    }
  }
  layer->parent = UI_NODE_NONE;
  layer->nextSibling = UI_NODE_NONE;
  layer->firstChild = UI_NODE_NONE;
  s_InGameKeyBindingsOverlayOpen = false;
  /* the keyboard focus goes back where it was (a node of the in-game tree, which outlives the overlay) */
  if (g_UiKeyboardFocusNode == UI_NODE_NONE || g_UiKeyboardFocusNode == layer) {
    UiKeyboardFocus_Set(s_InGameKeyBindingsOverlayPreviousFocus);
  }
  s_InGameKeyBindingsOverlayPreviousFocus = UI_NODE_NONE;
  Thandor_Log("in-game key bindings overlay: closed");
  UiNode_InvalidateRoot(root);
}

bool InGameKeyBindingsOverlay_IsOpen()
{
  return s_InGameKeyBindingsOverlayOpen;
}

/* Class vtable of the layer. */

UiNodeVtable g_InGameKeyBindingsLayerVtable = {
        .relocate = UI_SLOT(InGameKeyBindingsLayer_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren), /* no root flags: children */
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(InGameKeyBindingsLayer_IgnorePress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(InGameKeyBindingsLayer_IgnorePress),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(InGameKeyBindingsLayer_IgnorePress)};
