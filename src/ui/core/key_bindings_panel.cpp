/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/key_bindings_panel.cpp
 * Project code (not in the original game)
 */

/* The key binding panel (include/thandor/ui/core/key_bindings_panel.h).

   Nodes (module storage, linked at run time; nothing is relocated):
     root            g_KeyBindingsPanelVtable: the panel box; draws the optional background, the title and the
                     status line, lays out the children and fits the list into the scroll frame
       scroller      g_UiScrollableControlVtable: frame style A, a vertical bar when needed
         list        g_KeyBindingsPanelListVtable: the rows (content of the scroll frame); a focus target that
                     takes every key while a capture runs and polls the release in its tick (the keyboard focus
                     node ticks every frame tick)
       defaults      g_UiFramedTextButtonControlVtable, action KEY_BINDINGS_PANEL_ACTION_DEFAULTS
       back          g_UiFramedTextButtonControlVtable, action KEY_BINDINGS_PANEL_ACTION_BACK (also Esc)
   The two button actions are on the panel's own action handler page (KEY_BINDINGS_PANEL_ACTION_PAGE), installed by
   KeyBindingsPanel_Attach, so the panel works under the frontend and the in-game root alike. */

#include <thandor/ui/core/key_bindings_panel.h>
#include <thandor/thandor.h>
#include <thandor/core/settings/original_mode.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/ingame/layout.h>
#include <thandor/platform/input/devices.h>

#include <algorithm>
#include <cstring>
#include <iterator>

/* The panel's action handler page (page = action id >> 8; 0x30 is used by no other page). */
static constexpr UiActionHandlerPageIndex KEY_BINDINGS_PANEL_ACTION_PAGE = 0x30;
static constexpr UiActionId KEY_BINDINGS_PANEL_ACTION_DEFAULTS = 0x3000;
static constexpr UiActionId KEY_BINDINGS_PANEL_ACTION_BACK = 0x3001;

/* the rows: every visible action, one title row per section and a spacer row before every section but the first */
static constexpr int KEY_BINDINGS_PANEL_MAX_ROWS = KEY_ACTION_COUNT + 2 * KEY_BINDING_SECTION_COUNT;
static constexpr int32_t KEY_BINDINGS_PANEL_ROW_EXTRA = 8;     /* row height = font line height + this */
static constexpr int32_t KEY_BINDINGS_PANEL_TEXT_INSET = 6;    /* left inset of the description and the key text */
static constexpr int32_t KEY_BINDINGS_PANEL_CELL_PADDING = 16; /* key cell width = widest text + this */
static constexpr int32_t KEY_BINDINGS_PANEL_CELL_MIN_WIDTH = 140;
static constexpr int32_t KEY_BINDINGS_PANEL_CELL_GAP = 8;      /* between the description and the key cell */
static constexpr int32_t KEY_BINDINGS_PANEL_TITLE_HEIGHT = 26;
static constexpr int32_t KEY_BINDINGS_PANEL_STATUS_HEIGHT = 20;
static constexpr int32_t KEY_BINDINGS_PANEL_BUTTON_HEIGHT = 24;
static constexpr int32_t KEY_BINDINGS_PANEL_BUTTON_WIDTH = 112;
static constexpr int32_t KEY_BINDINGS_PANEL_GAP = 6;
static constexpr int32_t KEY_BINDINGS_PANEL_BACKGROUND_INSET = 12; /* children inset with KEY_BINDINGS_PANEL_BACKGROUND */
static constexpr uint32_t KEY_BINDINGS_PANEL_STATUS_UNITS = 192;
static constexpr UiAnchorFractionQ31 KEY_BINDINGS_PANEL_ANCHOR_HALF = 0x40000000;
static constexpr UiAnchorFractionQ31 KEY_BINDINGS_PANEL_ANCHOR_FULL = 0x80000000;

enum class KeyBindingsPanelRowKind : uint8_t { Spacer, Title, Action };
struct KeyBindingsPanelRow {
    KeyBindingsPanelRowKind kind;
    KeyBindingSection section; /* Title */
    KeyAction action;          /* Action */
};

struct KeyBindingsPanelNodes {
    UiPanelControl root;
    UiScrollableControl scroller;
    UiNodeBase list;
    UiFramedTextButtonControl defaultsButton;
    UiFramedTextButtonControl backButton;
};

extern UiNodeVtable g_KeyBindingsPanelVtable;
extern UiNodeVtable g_KeyBindingsPanelListVtable;

/* Module data. */

static KeyBindingsPanelNodes s_KeyBindingsPanel;
static bool s_KeyBindingsPanelAttached = false;
static uint32_t s_KeyBindingsPanelFlags = 0;
static KeyBindingsPanelCloseCallback s_KeyBindingsPanelOnClose = nullptr;
static void *s_KeyBindingsPanelContext = nullptr;
static KeyBindingsPanelRow s_KeyBindingsPanelRows[KEY_BINDINGS_PANEL_MAX_ROWS];
static int s_KeyBindingsPanelRowCount = 0;
static int32_t s_KeyBindingsPanelRowHeight = 22;
static int32_t s_KeyBindingsPanelKeyTextWidth = 0; /* widest key cell text (keys, "(keine)", the capture prompt) */
static int32_t s_KeyBindingsPanelCellWidth = KEY_BINDINGS_PANEL_CELL_MIN_WIDTH;
static uint16_t s_KeyBindingsPanelStatus[KEY_BINDINGS_PANEL_STATUS_UNITS];
static bool s_KeyBindingsPanelStatusIsHint = true; /* the status shows the hint (re-resolved on every draw) */
static bool s_KeyBindingsPanelStatusIsWarning = false;
static KeyCapture s_KeyBindingsPanelCapture = {};
static KeyAction s_KeyBindingsPanelCaptureAction = KeyAction::Count;
static UiActionHandlerPage s_KeyBindingsPanelActionPage = {};

/* ---- capture logic ---- */

bool KeyCapture_IsKeyDownCode(uint32_t keyCode)
{
  switch (keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) {
  case KEYBOARD_KEY_CODE_SPECIAL(0):
  case KEYBOARD_KEY_CODE_FUNCTION(0):
  case KEYBOARD_KEY_CODE_CHAR(0):
    return true;
  case 0:
    return keyCode == KEYBOARD_KEY_CODE_SPACE || keyCode == '*' || keyCode == '/' || keyCode == '+' || keyCode == '-';
  default:
    return false;
  }
}

void KeyCapture_Begin(KeyCapture *capture,uint8_t rangeLength)
{
  capture->active = true;
  capture->rangeLength = rangeLength;
  capture->keyCode = 0;
  capture->modifiers = KEYBOARD_STATE_NONE;
  capture->modifierPeak = KEYBOARD_STATE_NONE;
}

KeyCaptureResult KeyCapture_OnKeyEvent(KeyCapture *capture,uint32_t keyCode,UiKeyboardStateMask stateMask)
{
  const UiKeyboardStateMask modifiers = stateMask & KEYBOARD_STATE_ANY_MODIFIER;

  if (!capture->active || capture->keyCode != 0) {
    return KeyCaptureResult::Pending; /* swallowed: the key's character event, further keys */
  }
  if (keyCode == KEYBOARD_KEY_CODE_ESCAPE && modifiers == KEYBOARD_STATE_NONE) {
    capture->active = false;
    return KeyCaptureResult::Cancel;
  }
  if (!KeyCapture_IsKeyDownCode(keyCode)) {
    capture->active = false;
    return KeyCaptureResult::Rejected;
  }
  capture->keyCode = keyCode;
  capture->modifiers = modifiers;
  return KeyCaptureResult::Pending;
}

KeyCaptureResult KeyCapture_OnTick(KeyCapture *capture,UiKeyboardStateMask heldModifiers,bool keyHeld)
{
  const UiKeyboardStateMask held = heldModifiers & KEYBOARD_STATE_ANY_MODIFIER;

  if (!capture->active) {
    return KeyCaptureResult::Pending;
  }
  if (capture->keyCode == 0) {
    /* no key yet: a group row takes the modifiers alone once they are all released */
    if (held != KEYBOARD_STATE_NONE) {
      capture->modifierPeak = capture->modifierPeak | held;
      return KeyCaptureResult::Pending;
    }
    if (capture->rangeLength > 1 && capture->modifierPeak != KEYBOARD_STATE_NONE) {
      capture->keyCode = KEYBOARD_KEY_CODE_CHAR('1');
      capture->modifiers = capture->modifierPeak;
      capture->active = false;
      return KeyCaptureResult::Commit;
    }
    capture->modifierPeak = KEYBOARD_STATE_NONE;
    return KeyCaptureResult::Pending;
  }
  if (keyHeld) {
    capture->modifiers = capture->modifiers | held; /* the peak: modifiers pressed while the key is held */
    return KeyCaptureResult::Pending;
  }
  if (held != KEYBOARD_STATE_NONE) {
    return KeyCaptureResult::Pending;
  }
  capture->active = false;
  return KeyCaptureResult::Commit;
}

KeyChord KeyCapture_Chord(const KeyCapture *capture)
{
  UiKeyboardStateMask modifiers = KEYBOARD_STATE_NONE;

  if (Any(capture->modifiers & KEYBOARD_STATE_SHIFT)) {
    modifiers = modifiers | KEYBOARD_STATE_SHIFT;
  }
  if (Any(capture->modifiers & KEYBOARD_STATE_CTRL)) {
    modifiers = modifiers | KEYBOARD_STATE_CTRL;
  }
  if (Any(capture->modifiers & KEYBOARD_STATE_ALT)) {
    modifiers = modifiers | KEYBOARD_STATE_ALT;
  }
  return KeyChord{capture->keyCode,modifiers};
}

/* Whether the captured key is still held: the generic held state of every key-down code (Keyboard_KeyHeld), so a
   letter or function key is taken when it is released, together with every modifier held at the peak. */
static bool KeyBindingsPanel_KeyHeld(uint32_t keyCode)
{
  return Keyboard_KeyHeld(keyCode);
}

/* ---- texts ---- */

/* Appends text (UTF-16 with terminator; a leading rich-text command is dropped) to out at *length. */
static void KeyBindingsPanel_Append(uint16_t *out,uint32_t capacity,uint32_t *length,const uint16_t *text)
{
  if (text == nullptr || capacity == 0) {
    return;
  }
  if ((text[0] & RICHTEXT_COMMAND_FLAG) != 0) {
    text++;
  }
  for (; *text != 0 && *length + 1 < capacity; text++) {
    out[(*length)++] = *text;
  }
  out[*length] = 0;
}

static void KeyBindingsPanel_AppendText(uint16_t *out,uint32_t capacity,uint32_t *length,TextResourceId textId)
{
  KeyBindingsPanel_Append(out,capacity,length,TextResource_Resolve(textId));
}

/* The status line as ASCII for the log (other code units as '?'). */
static void KeyBindingsPanel_LogStatus(const char *what)
{
  char ascii[KEY_BINDINGS_PANEL_STATUS_UNITS];
  uint32_t index;

  for (index = 0; index + 1 < sizeof(ascii) && s_KeyBindingsPanelStatus[index] != 0; index++) {
    const uint16_t unit = s_KeyBindingsPanelStatus[index];
    ascii[index] = (unit >= 0x20 && unit < 0x7F) ? static_cast<char>(unit) : '?';
  }
  ascii[index] = '\0';
  Thandor_Log("key bindings panel: %s: %s",what,ascii);
}

KeyBindingResult KeyBindingsPanel_ApplyChord(KeyAction action,KeyChord chord,uint16_t *status,uint32_t capacity)
{
  const KeyBindingResult result = KeyBindings_Set(action,chord);
  uint32_t length = 0;

  if (capacity == 0) {
    return result;
  }
  status[0] = 0;
  switch (result.status) {
  case KeyBindingStatus::Ok: {
    const KeyBinding &binding = KeyBindings_Get(action);
    uint16_t keys[96];
    KeyBindingsPanel_AppendText(status,capacity,&length,binding.textId);
    static const uint16_t separator[] = {u':',u' ',0};
    KeyBindingsPanel_Append(status,capacity,&length,separator);
    if (KeyBinding_FormatKeys(binding,keys,static_cast<uint32_t>(std::size(keys))) == 0) {
      KeyBindingsPanel_AppendText(status,capacity,&length,TEXT_ID_KEYS_PANEL_UNBOUND);
    }
    else {
      KeyBindingsPanel_Append(status,capacity,&length,keys);
    }
    break;
  }
  case KeyBindingStatus::Conflict:
    KeyBindingsPanel_AppendText(status,capacity,&length,TEXT_ID_KEYS_PANEL_CONFLICT);
    if (result.conflictAction != KeyAction::Count) {
      KeyBindingsPanel_AppendText(status,capacity,&length,KeyBindings_Get(result.conflictAction).textId);
    }
    break;
  case KeyBindingStatus::NotAllowed:
  case KeyBindingStatus::Fixed:
    KeyBindingsPanel_AppendText(status,capacity,&length,TEXT_ID_KEYS_PANEL_NOT_ALLOWED);
    break;
  }
  return result;
}

/* Sets the status line to the hint (resolved when drawn, so it follows the language). */
static void KeyBindingsPanel_ShowHint()
{
  s_KeyBindingsPanelStatusIsHint = true;
  s_KeyBindingsPanelStatusIsWarning = false;
  s_KeyBindingsPanelStatus[0] = 0;
}

/* Sets the status line to textId followed by the description of action (KeyAction::Count: none). */
static void KeyBindingsPanel_ShowStatus(TextResourceId textId,KeyAction action,bool warning)
{
  uint32_t length = 0;

  s_KeyBindingsPanelStatus[0] = 0;
  KeyBindingsPanel_AppendText(s_KeyBindingsPanelStatus,KEY_BINDINGS_PANEL_STATUS_UNITS,&length,textId);
  if (action != KeyAction::Count) {
    KeyBindingsPanel_AppendText(s_KeyBindingsPanelStatus,KEY_BINDINGS_PANEL_STATUS_UNITS,&length,
                                KeyBindings_Get(action).textId);
  }
  s_KeyBindingsPanelStatusIsHint = false;
  s_KeyBindingsPanelStatusIsWarning = warning;
}

static int32_t KeyBindingsPanel_TextWidth(UiPackedTextStyle style,const uint16_t *text)
{
  return static_cast<int32_t>(RichTextCommandStream_MeasureLine(style,const_cast<uint16_t *>(text)).widthPixels);
}

/* ---- rows ---- */

static bool KeyBindingsPanel_Editable(KeyAction action)
{
  const KeyBinding &binding = KeyBindings_Get(action);
  return !KeyBinding_IsFixed(binding) && !KeyBinding_IsHidden(binding);
}

/* Builds the rows from the visible actions and measures the key texts. */
static void KeyBindingsPanel_BuildRows()
{
  bool haveSection = false;
  KeyBindingSection section = KeyBindingSection::InGameGeneral;
  int32_t widest = 0;
  uint16_t text[96];
  uint32_t lineHeight = 0;

  s_KeyBindingsPanelRowCount = 0;
  for (int index = 0; index < KeyBindings_Count(); index++) {
    const KeyAction action = static_cast<KeyAction>(index);
    const KeyBinding &binding = KeyBindings_Get(action);
    if (KeyBinding_IsHidden(binding)) {
      continue;
    }
    if (!haveSection || binding.section != section) {
      if (haveSection && s_KeyBindingsPanelRowCount < KEY_BINDINGS_PANEL_MAX_ROWS) {
        s_KeyBindingsPanelRows[s_KeyBindingsPanelRowCount++] = {KeyBindingsPanelRowKind::Spacer,section,KeyAction::Count};
      }
      haveSection = true;
      section = binding.section;
      if (s_KeyBindingsPanelRowCount < KEY_BINDINGS_PANEL_MAX_ROWS) {
        s_KeyBindingsPanelRows[s_KeyBindingsPanelRowCount++] = {KeyBindingsPanelRowKind::Title,section,KeyAction::Count};
      }
    }
    if (s_KeyBindingsPanelRowCount < KEY_BINDINGS_PANEL_MAX_ROWS) {
      s_KeyBindingsPanelRows[s_KeyBindingsPanelRowCount++] = {KeyBindingsPanelRowKind::Action,section,action};
    }
    if (KeyBinding_FormatKeys(binding,text,static_cast<uint32_t>(std::size(text))) != 0) {
      widest = std::max(widest,KeyBindingsPanel_TextWidth(g_UiListTextStyle,text));
    }
  }
  widest = std::max(widest,KeyBindingsPanel_TextWidth(g_UiListTextStyle,TextResource_Resolve(TEXT_ID_KEYS_PANEL_UNBOUND)));
  widest = std::max(widest,KeyBindingsPanel_TextWidth(g_UiTextStyleSelected,TextResource_Resolve(TEXT_ID_KEYS_PANEL_CAPTURE)));
  s_KeyBindingsPanelKeyTextWidth = widest;
  FontGlyph_GetLogicalSizeActiveFont(0,&lineHeight);
  s_KeyBindingsPanelRowHeight = static_cast<int32_t>(lineHeight) + KEY_BINDINGS_PANEL_ROW_EXTRA;
}

/* The row index at screen y within the list, -1 outside. */
static int KeyBindingsPanel_RowAt(UiPixelCoordinate pointerY)
{
  const UiNodeBase &list = s_KeyBindingsPanel.list;
  int row;

  if (pointerY < list.top || s_KeyBindingsPanelRowHeight <= 0) {
    return -1;
  }
  row = (pointerY - list.top) / s_KeyBindingsPanelRowHeight;
  return row < s_KeyBindingsPanelRowCount ? row : -1;
}

/* ---- capture in the panel ---- */

static void KeyBindingsPanel_Invalidate()
{
  if (s_KeyBindingsPanelAttached) {
    UiNode_InvalidateRoot(&s_KeyBindingsPanel.root.root.base);
  }
}

static void KeyBindingsPanel_CancelCapture()
{
  if (s_KeyBindingsPanelCapture.active) {
    s_KeyBindingsPanelCapture.active = false;
    s_KeyBindingsPanelCaptureAction = KeyAction::Count;
    KeyBindingsPanel_ShowHint();
    KeyBindingsPanel_Invalidate();
  }
}

static void KeyBindingsPanel_BeginCapture(KeyAction action)
{
  KeyCapture_Begin(&s_KeyBindingsPanelCapture,KeyBindings_Get(action).rangeLength);
  s_KeyBindingsPanelCaptureAction = action;
  KeyBindingsPanel_ShowHint();
  if (g_UiKeyboardFocusNode != &s_KeyBindingsPanel.list) {
    UiKeyboardFocus_Set(&s_KeyBindingsPanel.list);
  }
  Thandor_Log("key bindings panel: capture %s",KeyBindings_Name(action));
  KeyBindingsPanel_Invalidate();
}

/* A capture ended with result (not Pending): apply or report it. */
static void KeyBindingsPanel_FinishCapture(KeyCaptureResult result)
{
  const KeyAction action = s_KeyBindingsPanelCaptureAction;

  s_KeyBindingsPanelCaptureAction = KeyAction::Count;
  s_KeyBindingsPanelCapture.active = false;
  switch (result) {
  case KeyCaptureResult::Commit: {
    const KeyBindingResult applied = KeyBindingsPanel_ApplyChord
              (action,KeyCapture_Chord(&s_KeyBindingsPanelCapture),s_KeyBindingsPanelStatus,KEY_BINDINGS_PANEL_STATUS_UNITS);
    s_KeyBindingsPanelStatusIsHint = false;
    s_KeyBindingsPanelStatusIsWarning = applied.status != KeyBindingStatus::Ok;
    KeyBindingsPanel_LogStatus(KeyBindings_Name(action));
    break;
  }
  case KeyCaptureResult::Rejected:
    KeyBindingsPanel_ShowStatus(TEXT_ID_KEYS_PANEL_NOT_ALLOWED,KeyAction::Count,true);
    KeyBindingsPanel_LogStatus(KeyBindings_Name(action));
    break;
  case KeyCaptureResult::Cancel:
  case KeyCaptureResult::Pending:
    KeyBindingsPanel_ShowHint();
    Thandor_Log("key bindings panel: capture cancelled");
    break;
  }
  KeyBindingsPanel_Invalidate();
}

/* A capture runs only while the list has the keyboard focus (a click elsewhere ends it). */
static bool KeyBindingsPanel_CheckCaptureFocus()
{
  if (s_KeyBindingsPanelCapture.active && g_UiKeyboardFocusNode != &s_KeyBindingsPanel.list) {
    KeyBindingsPanel_CancelCapture();
  }
  return s_KeyBindingsPanelCapture.active;
}

bool KeyBindingsPanel_Capturing()
{
  return s_KeyBindingsPanelAttached && KeyBindingsPanel_CheckCaptureFocus();
}

/* ---- list node (g_KeyBindingsPanelListVtable) ---- */

static void KeyBindingsPanelList_Draw
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
           UiNodeBase *node)
{
  const int32_t rowHeight = s_KeyBindingsPanelRowHeight;
  uint32_t lineHeight = 0;
  int first;
  int last;
  uint16_t text[96];

  if (s_KeyBindingsPanelRowCount == 0 || rowHeight <= 0) {
    return;
  }
  KeyBindingsPanel_CheckCaptureFocus();
  clipLeft = std::max(clipLeft,node->left);
  clipTop = std::max(clipTop,node->top);
  clipRight = std::min(clipRight,node->right);
  clipBottom = std::min(clipBottom,node->bottom);
  if (clipLeft >= clipRight || clipTop >= clipBottom) {
    return;
  }
  first = std::max(0,(clipTop - node->top) / rowHeight);
  last = std::min(s_KeyBindingsPanelRowCount - 1,(clipBottom - node->top) / rowHeight);
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  FontGlyph_GetLogicalSizeActiveFont(0,&lineHeight);
  const int32_t textOffsetY = (rowHeight - static_cast<int32_t>(lineHeight)) / 2;
  const int32_t cellRight = node->right - 2;
  const int32_t cellLeft = cellRight - s_KeyBindingsPanelCellWidth;
  for (int index = first; index <= last; index++) {
    const KeyBindingsPanelRow &row = s_KeyBindingsPanelRows[index];
    const int32_t rowTop = node->top + index * rowHeight;
    if (row.kind == KeyBindingsPanelRowKind::Spacer) {
      continue;
    }
    if (row.kind == KeyBindingsPanelRowKind::Title) {
      RichTextCommandStream_DrawSingleLine(clipBottom,clipRight,clipTop,clipLeft,g_UiTextStyleSelected,
                                           TextResource_Resolve(KeyBindings_SectionTitle(row.section)),
                                           rowTop + textOffsetY,node->left + KEY_BINDINGS_PANEL_TEXT_INSET - 2);
      continue;
    }
    const KeyBinding &binding = KeyBindings_Get(row.action);
    const bool fixed = KeyBinding_IsFixed(binding);
    const bool capturing = s_KeyBindingsPanelCapture.active && s_KeyBindingsPanelCaptureAction == row.action;
    const UiPackedTextStyle style = fixed ? g_UiTextStyleDisabled : g_UiListTextStyle;
    /* the description, cut at the key cell */
    RichTextCommandStream_DrawSingleLine(clipBottom,std::min(clipRight,cellLeft - KEY_BINDINGS_PANEL_CELL_GAP),clipTop,
                                         clipLeft,style,TextResource_Resolve(binding.textId),rowTop + textOffsetY,
                                         node->left + KEY_BINDINGS_PANEL_TEXT_INSET);
    /* the key cell: an inset frame (selected while capturing, disabled when fixed) with the keys in it */
    const UiPixelRect cell = {cellLeft,rowTop + 1,cellRight,rowTop + rowHeight - 1};
    GraphicsSubresourceIndex frame = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    if (fixed) {
      frame = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
    else if (capturing) {
      frame = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME + UI_WINDOW_FRAME_PIECE_COUNT;
    }
    UiFramedFrame_Draw(cell,frame,UiPixelRect{clipLeft,clipTop,clipRight,clipBottom});
    const uint16_t *keys;
    UiPackedTextStyle keyStyle = style;
    if (capturing) {
      keys = TextResource_Resolve(TEXT_ID_KEYS_PANEL_CAPTURE);
      keyStyle = g_UiTextStyleSelected;
    }
    else if (KeyBinding_FormatKeys(binding,text,static_cast<uint32_t>(std::size(text))) != 0) {
      keys = text;
    }
    else {
      keys = TextResource_Resolve(TEXT_ID_KEYS_PANEL_UNBOUND);
      keyStyle = g_UiTextStyleDisabled;
    }
    RichTextCommandStream_DrawSingleLine(std::min(clipBottom,cell.bottom - 2),std::min(clipRight,cell.right - 4),
                                         std::max(clipTop,cell.top + 2),std::max(clipLeft,cell.left + 4),keyStyle,
                                         const_cast<uint16_t *>(keys),rowTop + textOffsetY,
                                         cell.left + KEY_BINDINGS_PANEL_TEXT_INSET);
  }
  g_GraphicsFramebufferEndAccess();
}

/* A left press on an editable row starts its capture (a press on another row moves it there). */
static void KeyBindingsPanelList_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *node)
{
  (void)wheelDelta;
  (void)pointerX;
  (void)node;
  const int row = KeyBindingsPanel_RowAt(pointerY);

  if (row < 0 || s_KeyBindingsPanelRows[row].kind != KeyBindingsPanelRowKind::Action ||
      !KeyBindingsPanel_Editable(s_KeyBindingsPanelRows[row].action)) {
    KeyBindingsPanel_CancelCapture();
    return;
  }
  KeyBindingsPanel_BeginCapture(s_KeyBindingsPanelRows[row].action);
}

/* A right press on an editable row clears its binding (the press is not passed to the scroll frame). */
static void KeyBindingsPanelList_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *node)
{
  (void)wheelDelta;
  (void)pointerX;
  (void)node;
  const int row = KeyBindingsPanel_RowAt(pointerY);

  KeyBindingsPanel_CancelCapture();
  if (row < 0 || s_KeyBindingsPanelRows[row].kind != KeyBindingsPanelRowKind::Action ||
      !KeyBindingsPanel_Editable(s_KeyBindingsPanelRows[row].action)) {
    return;
  }
  const KeyAction action = s_KeyBindingsPanelRows[row].action;
  if (KeyBindings_Clear(action)) {
    KeyBindingsPanel_ShowStatus(TEXT_ID_KEYS_PANEL_CLEARED,action,false);
    KeyBindingsPanel_LogStatus(KeyBindings_Name(action));
  }
  KeyBindingsPanel_Invalidate();
}

/* While a capture runs every key is taken (false); otherwise keys go on (Tab, Esc to "Zurueck"). */
static bool KeyBindingsPanelList_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
                                               UiNodeBase *node)
{
  (void)node;
  if (!KeyBindingsPanel_CheckCaptureFocus()) {
    return true;
  }
  const KeyCaptureResult result = KeyCapture_OnKeyEvent(&s_KeyBindingsPanelCapture,keyCode,keyboardStateMask);
  if (result != KeyCaptureResult::Pending) {
    KeyBindingsPanel_FinishCapture(result);
  }
  return false;
}

/* Every frame tick while the list has the focus: a capture ends when its keys are released. */
static void KeyBindingsPanelList_Tick(UiNodeBase *node)
{
  (void)node;
  if (!KeyBindingsPanel_CheckCaptureFocus()) {
    return;
  }
  const KeyCaptureResult result =
            KeyCapture_OnTick(&s_KeyBindingsPanelCapture,g_KeyboardStateMask,
                              KeyBindingsPanel_KeyHeld(s_KeyBindingsPanelCapture.keyCode));
  if (result != KeyCaptureResult::Pending) {
    KeyBindingsPanel_FinishCapture(result);
  }
}

static void KeyBindingsPanelList_Layout(UiNodeBase *node)
{
  node->layoutWidth = node->right - node->left;
  node->layoutHeight = node->bottom - node->top;
}

static UiNodeBase *KeyBindingsPanelList_HitTest(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *node)
{
  (void)pointerY;
  (void)pointerX;
  return node;
}

static void KeyBindingsPanel_Relocate(UiSerializedRelocationDelta relocationDelta,UiNodeBase *node)
{
  /* module nodes linked at run time: nothing to relocate */
  (void)relocationDelta;
  (void)node;
}

/* ---- root node (g_KeyBindingsPanelVtable) ---- */

static int32_t KeyBindingsPanel_Inset()
{
  return (s_KeyBindingsPanelFlags & KEY_BINDINGS_PANEL_BACKGROUND) != 0 ? KEY_BINDINGS_PANEL_BACKGROUND_INSET : 0;
}

static int32_t KeyBindingsPanel_TitleHeight()
{
  return (s_KeyBindingsPanelFlags & KEY_BINDINGS_PANEL_TITLE) != 0 ? KEY_BINDINGS_PANEL_TITLE_HEIGHT : 0;
}

static void KeyBindingsPanel_Draw
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
           UiNodeBase *node)
{
  UiPanelControl *panel = &s_KeyBindingsPanel.root;
  const int32_t inset = KeyBindingsPanel_Inset();
  uint32_t lineHeight = 0;

  UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren(clipBottom,clipRight,clipTop,clipLeft,panel);
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  FontGlyph_GetLogicalSizeActiveFont(0,&lineHeight);
  const UiPixelCoordinate textClipLeft = std::max(clipLeft,node->left + inset);
  const UiPixelCoordinate textClipRight = std::min(clipRight,node->right - inset);
  if ((s_KeyBindingsPanelFlags & KEY_BINDINGS_PANEL_TITLE) != 0) {
    uint16_t *title = TextResource_Resolve(TEXT_ID_KEYS_TITLE);
    const int32_t width = KeyBindingsPanel_TextWidth(g_UiTextStyleNormal,title);
    RichTextCommandStream_DrawSingleLine(clipBottom,textClipRight,clipTop,textClipLeft,g_UiTextStyleNormal,title,
                                         node->top + inset + (KEY_BINDINGS_PANEL_TITLE_HEIGHT - (int32_t)lineHeight) / 2,
                                         node->left + (node->right - node->left - width) / 2);
  }
  /* the status line between the list and the buttons */
  const int32_t statusTop = node->bottom - inset - KEY_BINDINGS_PANEL_BUTTON_HEIGHT - KEY_BINDINGS_PANEL_GAP -
                            KEY_BINDINGS_PANEL_STATUS_HEIGHT;
  uint16_t *status = s_KeyBindingsPanelStatusIsHint ? TextResource_Resolve(TEXT_ID_KEYS_PANEL_HINT)
                                                    : s_KeyBindingsPanelStatus;
  RichTextCommandStream_DrawSingleLine(std::min(clipBottom,statusTop + KEY_BINDINGS_PANEL_STATUS_HEIGHT),textClipRight,
                                       std::max(clipTop,statusTop),textClipLeft,
                                       s_KeyBindingsPanelStatusIsWarning ? g_UiTextStyleSelected : g_UiTextStyleNormal,
                                       status,statusTop + (KEY_BINDINGS_PANEL_STATUS_HEIGHT - (int32_t)lineHeight) / 2,
                                       node->left + inset + 2);
  g_GraphicsFramebufferEndAccess();
}

/* The list fills the scroll frame's width (less the frame and the vertical bar); the key cell takes the widest
   key text, at most half the list. */
static void KeyBindingsPanel_FitList()
{
  UiScrollableControl *scroller = &s_KeyBindingsPanel.scroller;
  const GraphicsTextureLogicalSize frame =
            g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FRAME_A_FIRST,g_UiWindowTextureSource);
  const GraphicsTextureLogicalSize bar =
            g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_VERTICAL_ARROW,g_UiWindowTextureSource);
  const int32_t width = std::max<int32_t>(0,(scroller->base.right - scroller->base.left) -
                                             2 * (int32_t)frame.logicalWidthPixels - (int32_t)bar.logicalWidthPixels);

  s_KeyBindingsPanel.list.rightOffset = width;
  s_KeyBindingsPanel.list.bottomOffset = s_KeyBindingsPanelRowCount * s_KeyBindingsPanelRowHeight;
  s_KeyBindingsPanelCellWidth = std::clamp(s_KeyBindingsPanelKeyTextWidth + KEY_BINDINGS_PANEL_CELL_PADDING,
                                           KEY_BINDINGS_PANEL_CELL_MIN_WIDTH,std::max(KEY_BINDINGS_PANEL_CELL_MIN_WIDTH,
                                                                                      width / 2));
  scroller->base.vtable->layout(&scroller->base);
}

static void KeyBindingsPanel_Layout(UiNodeBase *node)
{
  UiContainer_LayoutChildren(node);
  KeyBindingsPanel_FitList();
}

/* ---- buttons ---- */

static void KeyBindingsPanelAction_Defaults(void *source)
{
  (void)source;
  KeyBindingsPanel_CancelCapture();
  KeyBindings_ResetAllDefaults();
  KeyBindingsPanel_ShowStatus(TEXT_ID_KEYS_PANEL_DEFAULTS_RESTORED,KeyAction::Count,false);
  KeyBindingsPanel_LogStatus("defaults");
  KeyBindingsPanel_Refresh();
}

static void KeyBindingsPanelAction_Back(void *source)
{
  (void)source;
  KeyBindingsPanel_CancelCapture();
  Thandor_Log("key bindings panel: back");
  if (s_KeyBindingsPanelOnClose != nullptr) {
    s_KeyBindingsPanelOnClose(s_KeyBindingsPanelContext);
  }
}

/* ---- attach / detach ---- */

static void KeyBindingsPanel_OnBindingsChanged()
{
  KeyBindingsPanel_Refresh();
}

/* "Original Thandor" switched: the keys shown (KeyBinding_FormatKeys, the acting keys) changed */
static void KeyBindingsPanel_OnOriginalModeChanged(bool originalModeOn)
{
  (void)originalModeOn;
  KeyBindingsPanel_Refresh(); /* nothing while not attached */
}

/* Sets a node's links, vtable, anchors and offsets. */
static void KeyBindingsPanel_InitNode(UiNodeBase *node,UiNodeVtable *vtable,UiNodeBase *parent,UiNodeBase *nextSibling,
                                      UiNodeBase *firstChild,UiAnchorFractionQ31 leftAnchor,int32_t leftOffset,
                                      UiAnchorFractionQ31 topAnchor,int32_t topOffset,UiAnchorFractionQ31 rightAnchor,
                                      int32_t rightOffset,UiAnchorFractionQ31 bottomAnchor,int32_t bottomOffset,
                                      UiNodeFlags flags)
{
  std::memset(static_cast<void *>(node),0,sizeof(*node));
  node->vtable = vtable;
  node->parent = parent;
  node->nextSibling = nextSibling;
  node->firstChild = firstChild;
  node->leftAnchorQ31 = leftAnchor;
  node->leftOffset = leftOffset;
  node->topAnchorQ31 = topAnchor;
  node->topOffset = topOffset;
  node->rightAnchorQ31 = rightAnchor;
  node->rightOffset = rightOffset;
  node->bottomAnchorQ31 = bottomAnchor;
  node->bottomOffset = bottomOffset;
  node->nodeFlags = flags;
}

static void KeyBindingsPanel_InitButton(UiFramedTextButtonControl *button,UiNodeBase *nextSibling,
                                        UiAnchorFractionQ31 horizontalAnchor,int32_t left,int32_t right,
                                        UiActionId actionId,UiSelectableStateFlags stateFlags,TextResourceId textId)
{
  const int32_t inset = KeyBindingsPanel_Inset();

  std::memset(static_cast<void *>(button),0,sizeof(*button));
  KeyBindingsPanel_InitNode(&button->selectable.base,&g_UiFramedTextButtonControlVtable,
                            &s_KeyBindingsPanel.root.root.base,nextSibling,UI_NODE_NONE,horizontalAnchor,left,
                            KEY_BINDINGS_PANEL_ANCHOR_FULL,-inset - KEY_BINDINGS_PANEL_BUTTON_HEIGHT,horizontalAnchor,
                            right,KEY_BINDINGS_PANEL_ANCHOR_FULL,-inset,UI_NODE_FALLBACK_FOCUS_TARGET);
  button->selectable.stateFlags = stateFlags;
  button->selectable.actionId = actionId;
  button->textResourceId = textId;
  button->activationSound = g_UiButtonSoundVoiceSets7[3];
}

/* Builds the node tree for the current flags (the root's box is set by Attach). */
static void KeyBindingsPanel_BuildNodes()
{
  KeyBindingsPanelNodes &nodes = s_KeyBindingsPanel;
  const int32_t inset = KeyBindingsPanel_Inset();
  const int32_t listTop = inset + KeyBindingsPanel_TitleHeight();
  const int32_t listBottom = -inset - KEY_BINDINGS_PANEL_BUTTON_HEIGHT - KEY_BINDINGS_PANEL_GAP -
                             KEY_BINDINGS_PANEL_STATUS_HEIGHT - KEY_BINDINGS_PANEL_GAP;

  std::memset(static_cast<void *>(&nodes.scroller),0,sizeof(nodes.scroller));
  KeyBindingsPanel_InitNode(&nodes.scroller.base,&g_UiScrollableControlVtable,&nodes.root.root.base,
                            &nodes.defaultsButton.selectable.base,&nodes.list,0,inset,0,listTop,
                            KEY_BINDINGS_PANEL_ANCHOR_FULL,-inset,KEY_BINDINGS_PANEL_ANCHOR_FULL,listBottom,
                            FromBits<UiNodeFlags>(0));
  /* frame style A, a vertical bar at the right when needed (no horizontal bar: the list fits the width) */
  nodes.scroller.scrollStateFlags = UI_SCROLL_FRAME_STYLE_A | FromBits<UiScrollableStateFlags>(0x80);
  nodes.scroller.autoScrollStepX = 15;
  nodes.scroller.autoScrollStepY = 15;
  /* the list: its size is its right / bottom offsets (KeyBindingsPanel_FitList) */
  KeyBindingsPanel_InitNode(&nodes.list,&g_KeyBindingsPanelListVtable,&nodes.scroller.base,UI_NODE_NONE,UI_NODE_NONE,0,0,
                            0,0,0,0,0,0,UI_NODE_FALLBACK_FOCUS_TARGET);
  KeyBindingsPanel_InitButton(&nodes.defaultsButton,&nodes.backButton.selectable.base,0,inset,
                              inset + KEY_BINDINGS_PANEL_BUTTON_WIDTH,KEY_BINDINGS_PANEL_ACTION_DEFAULTS,
                              UI_BUTTON_PLAY_ACTIVATION_SOUND,TEXT_ID_KEYS_PANEL_BUTTON_DEFAULTS);
  /* "Zurueck" as the pages' back buttons: Enter and Esc activate it */
  KeyBindingsPanel_InitButton(&nodes.backButton,UI_NODE_NONE,KEY_BINDINGS_PANEL_ANCHOR_FULL,
                              -inset - KEY_BINDINGS_PANEL_BUTTON_WIDTH,-inset,KEY_BINDINGS_PANEL_ACTION_BACK,
                              UI_BUTTON_PLAY_ACTIVATION_SOUND | UI_SELECTABLE_ACTIVATE_ON_ENTER |
                              UI_SELECTABLE_ACTIVATE_ON_ESCAPE,
                              TEXT_ID_KEYS_PANEL_BUTTON_BACK);
}

void KeyBindingsPanel_Attach(UiNodeBase *parent,UiPixelRect rect,uint32_t flags,KeyBindingsPanelCloseCallback onClose,
                             void *context)
{
  KeyBindingsPanelNodes &nodes = s_KeyBindingsPanel;
  UiNodeBase *root = &nodes.root.root.base;
  const UiAnchorFractionQ31 anchor = (flags & KEY_BINDINGS_PANEL_CENTERED) != 0 ? KEY_BINDINGS_PANEL_ANCHOR_HALF : 0;

  if (parent == nullptr || parent == UI_NODE_NONE) {
    return;
  }
  KeyBindingsPanel_Detach();
  s_KeyBindingsPanelFlags = flags;
  s_KeyBindingsPanelOnClose = onClose;
  s_KeyBindingsPanelContext = context;
  s_KeyBindingsPanelCapture = {};
  s_KeyBindingsPanelCaptureAction = KeyAction::Count;
  KeyBindingsPanel_ShowHint();

  std::memset(static_cast<void *>(&nodes.root),0,sizeof(nodes.root));
  KeyBindingsPanel_InitNode(root,&g_KeyBindingsPanelVtable,parent,UI_NODE_NONE,&nodes.scroller.base,anchor,rect.left,
                            anchor,rect.top,anchor,rect.right,anchor,rect.bottom,FromBits<UiNodeFlags>(0));
  if ((flags & KEY_BINDINGS_PANEL_BACKGROUND) != 0) {
    nodes.root.root.rootFlags = UI_ROOT_TILED_BACKGROUND | UI_ROOT_FRAME;
  }
  KeyBindingsPanel_BuildNodes();

  /* the action page of the two buttons */
  s_KeyBindingsPanelActionPage.handlers[KEY_BINDINGS_PANEL_ACTION_DEFAULTS & 0xFF] = &KeyBindingsPanelAction_Defaults;
  s_KeyBindingsPanelActionPage.handlers[KEY_BINDINGS_PANEL_ACTION_BACK & 0xFF] = &KeyBindingsPanelAction_Back;
  UiActionHandlers_SetPage(KEY_BINDINGS_PANEL_ACTION_PAGE,&s_KeyBindingsPanelActionPage);

  /* the last child of parent: drawn last, hit first */
  if (parent->firstChild == UI_NODE_NONE) {
    parent->firstChild = root;
  }
  else {
    UiNodeBase *last = parent->firstChild;
    while (last->nextSibling != UI_NODE_NONE) {
      last = last->nextSibling;
    }
    last->nextSibling = root;
  }
  s_KeyBindingsPanelAttached = true;
  KeyBindings_AddChangeListener(&KeyBindingsPanel_OnBindingsChanged);
  static bool originalModeListenerAdded = false; /* the master switch has no remove: added once */
  if (!originalModeListenerAdded) {
    originalModeListenerAdded = true;
    OriginalMode_AddChangeListener(&KeyBindingsPanel_OnOriginalModeChanged);
  }

  /* the box from the parent's current box (UiContainer_LayoutChildren's rule), then the subtree */
  const int32_t parentWidth = parent->right - parent->left;
  const int32_t parentHeight = parent->bottom - parent->top;
  const int32_t anchorX = anchor != 0 ? parentWidth / 2 : 0;
  const int32_t anchorY = anchor != 0 ? parentHeight / 2 : 0;
  root->left = parent->left + anchorX + rect.left;
  root->top = parent->top + anchorY + rect.top;
  root->right = parent->left + anchorX + rect.right;
  root->bottom = parent->top + anchorY + rect.bottom;
  KeyBindingsPanel_BuildRows();
  nodes.scroller.scrollOffsetX = 0;
  nodes.scroller.scrollOffsetY = 0;
  root->vtable->layout(root);
  UiKeyboardFocus_Set(&nodes.list);
  Thandor_Log("key bindings panel: attached (%d,%d)-(%d,%d), %d rows",root->left,root->top,root->right,root->bottom,
              s_KeyBindingsPanelRowCount);
  UiNode_InvalidateRoot(root);
}

/* True when node lies in the panel's subtree. */
static bool KeyBindingsPanel_Contains(UiNodeBase *node)
{
  for (; node != nullptr && node != UI_NODE_NONE; node = node->parent) {
    if (node == &s_KeyBindingsPanel.root.root.base) {
      return true;
    }
  }
  return false;
}

void KeyBindingsPanel_Detach()
{
  UiNodeBase *root = &s_KeyBindingsPanel.root.root.base;
  UiNodeBase *parent;
  bool hadFocus;

  if (!s_KeyBindingsPanelAttached) {
    return;
  }
  s_KeyBindingsPanelCapture.active = false;
  s_KeyBindingsPanelCaptureAction = KeyAction::Count;
  KeyBindings_RemoveChangeListener(&KeyBindingsPanel_OnBindingsChanged);
  hadFocus = KeyBindingsPanel_Contains(g_UiKeyboardFocusNode);
  if (KeyBindingsPanel_Contains(g_UiPointerCaptureTarget)) {
    g_UiPointerCaptureTarget = UI_NODE_NONE;
    g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  }
  /* UiRootStack_Pop drops the tooltip target only when its root chain ends at the closing root; a detached
     subtree ends at the panel root, so a target in here would outlive the host root */
  if (KeyBindingsPanel_Contains(g_UiTooltipState.targetNode)) {
    g_UiTooltipState.targetNode = nullptr;
  }
  if (g_UiImageControlHoverTarget != nullptr &&
      KeyBindingsPanel_Contains(&g_UiImageControlHoverTarget->selectable.base)) {
    g_UiImageControlHoverTarget = nullptr;
  }
  parent = root->parent;
  if (parent->firstChild == root) {
    parent->firstChild = root->nextSibling;
  }
  else {
    for (UiNodeBase *node = parent->firstChild; node != UI_NODE_NONE; node = node->nextSibling) {
      if (node->nextSibling == root) {
        node->nextSibling = root->nextSibling;
        break;
      }
    }
  }
  root->nextSibling = UI_NODE_NONE;
  root->parent = UI_NODE_NONE;
  s_KeyBindingsPanelAttached = false;
  if (hadFocus) {
    UiKeyboardFocus_Set(UI_NODE_NONE);
    UiKeyboardFocus_SelectInitial(parent);
  }
  Thandor_Log("key bindings panel: detached");
  UiNode_InvalidateRoot(parent);
}

void KeyBindingsPanel_Refresh()
{
  if (!s_KeyBindingsPanelAttached) {
    return;
  }
  KeyBindingsPanel_BuildRows();
  KeyBindingsPanel_FitList();
  KeyBindingsPanel_Invalidate();
}

bool KeyBindingsPanel_IsAttached()
{
  return s_KeyBindingsPanelAttached;
}

UiNodeBase *KeyBindingsPanel_Root()
{
  return &s_KeyBindingsPanel.root.root.base;
}

/* Class vtables. */

UiNodeVtable g_KeyBindingsPanelVtable = {
        .relocate = UI_SLOT(KeyBindingsPanel_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(KeyBindingsPanel_Draw),
        .layout = UI_SLOT(KeyBindingsPanel_Layout),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
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
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_KeyBindingsPanelListVtable = {
        .relocate = UI_SLOT(KeyBindingsPanel_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(KeyBindingsPanelList_Draw),
        .layout = UI_SLOT(KeyBindingsPanelList_Layout),
        .nonRightPress = UI_SLOT(KeyBindingsPanelList_NonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(KeyBindingsPanelList_RightPress),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(KeyBindingsPanelList_HitTest),
        .keyboardEvent = UI_SLOT(KeyBindingsPanelList_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(KeyBindingsPanelList_Tick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};
