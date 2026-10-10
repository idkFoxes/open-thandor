/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/key_bindings_page.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/frontend/key_bindings_page.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/text/richtext.h>
#include <thandor/ui/core/key_bindings.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iterator>

/* One list row (the record a row slot points at): column 0 at keys, column 1 at action, both rich-text streams.
   A key row holds the formatted keys and a jump to the action text (a binding with many keys continues in rows
   without action text); a section row its title behind a palette 1
   command (the selected text style, g_UiTextStyleSelected) and an empty action; a spacer row (before every section
   but the first) is empty. The jump record keeps the resolved text where it is (the TEXT_ID_KEYS_* texts are static
   module data, so their address fits the record's 32-bit pointer). action: the jump command
   (RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED, or 0 for an empty text), the 32-bit text pointer and the 32-bit
   selector, RICHTEXT_RECORD_UNITS_NESTED code units, then a terminator (not reached after a jump). */
struct KeyBindingsPageRow {
    uint16_t keys[64];
    uint16_t action[RICHTEXT_RECORD_UNITS_NESTED + 1];
};

/* the table rows, one title row per section and a spacer row before every section but the first */
static constexpr uint32_t KEY_BINDINGS_PAGE_MAX_ROWS = 96;
static constexpr int32_t KEY_BINDINGS_PAGE_KEY_LINE_WIDTH = 150; /* longer key lists continue in the next row */
static constexpr int32_t KEY_BINDINGS_PAGE_KEY_COLUMN_GAP = 16; /* between the longest key text and the actions */
static constexpr int32_t KEY_BINDINGS_PAGE_ACTION_COLUMN_GAP = 6;

/* Module data. */

static KeyBindingsPageRow s_keyBindingsPageRows[KEY_BINDINGS_PAGE_MAX_ROWS];
static Ptr32<void> s_keyBindingsPageRowSlots[KEY_BINDINGS_PAGE_MAX_ROWS];

/* The frontend root of any of its nodes. */
static UiNodeBase *FrontendKeyBindingsPage_Root(UiNodeBase *node)
{
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  return node;
}

/* Width in pixels of a row column's text in the list style. */
static int32_t FrontendKeyBindingsPage_TextWidth(uint16_t *text)
{
  return (int32_t)RichTextCommandStream_MeasureLine(g_UiListTextStyle,text).widthPixels;
}

/* Writes a jump record to text (with a zero selector) into record. */
static void FrontendKeyBindingsPage_SetJump(uint16_t (&record)[RICHTEXT_RECORD_UNITS_NESTED + 1],uint16_t *text)
{
  const Ptr32<uint16_t> pointer = text;
  const uint32_t selector = 0;

  record[0] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
  std::memcpy(&record[1],&pointer.value,sizeof(pointer.value));
  std::memcpy(&record[3],&selector,sizeof(selector));
  record[RICHTEXT_RECORD_UNITS_NESTED] = 0;
}

/* Builds the rows from KeyBindings_Table; returns their number. */
static uint32_t FrontendKeyBindingsPage_BuildRows(int32_t *outKeyWidth,int32_t *outActionWidth)
{
  uint32_t bindingCount = 0;
  const KeyBinding *bindings = KeyBindings_Table(&bindingCount);
  uint32_t rowCount = 0;
  int32_t keyWidth = 0;
  int32_t actionWidth = 0;
  bool haveSection = false;
  KeyBindingSection section = KeyBindingSection::InGameGeneral;

  for (uint32_t index = 0; index < bindingCount; index++) {
    const KeyBinding &binding = bindings[index];
    if (!haveSection || binding.section != section) {
      if (haveSection && rowCount < KEY_BINDINGS_PAGE_MAX_ROWS) {
        KeyBindingsPageRow &spacer = s_keyBindingsPageRows[rowCount++];
        spacer.keys[0] = 0;
        spacer.action[0] = 0;
      }
      haveSection = true;
      section = binding.section;
      if (rowCount < KEY_BINDINGS_PAGE_MAX_ROWS) {
        KeyBindingsPageRow &title = s_keyBindingsPageRows[rowCount++];
        const uint16_t *titleText = TextResource_Resolve(KeyBindings_SectionTitle(section));
        uint32_t length = 0;
        title.keys[0] = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1;
        while (titleText != nullptr && titleText[length] != 0 && length + 2 < std::size(title.keys)) {
          title.keys[length + 1] = titleText[length];
          length++;
        }
        title.keys[length + 1] = 0;
        title.action[0] = 0;
      }
    }
    if (rowCount >= KEY_BINDINGS_PAGE_MAX_ROWS) {
      break;
    }
    /* the keys, joined with " / "; a line that would get wider than KEY_BINDINGS_PAGE_KEY_LINE_WIDTH ends after
       the separator and the next key starts a continuation row (without action text) */
    KeyBindingsPageRow *row = &s_keyBindingsPageRows[rowCount++];
    uint32_t length = 0;
    row->keys[0] = 0;
    FrontendKeyBindingsPage_SetJump(row->action,TextResource_Resolve(binding.textId));
    actionWidth = std::max(actionWidth,FrontendKeyBindingsPage_TextWidth(row->action));
    for (int keyIndex = 0; keyIndex < KEY_BINDING_MAX_KEYS && binding.keys[keyIndex].keyCode != 0; keyIndex++) {
      KeyBinding single = binding;
      uint16_t name[64];
      uint16_t candidate[64];
      uint32_t nameLength;
      uint32_t candidateLength;

      for (int other = 1; other < KEY_BINDING_MAX_KEYS; other++) {
        single.keys[other].keyCode = 0;
      }
      single.keys[0] = binding.keys[keyIndex];
      nameLength = KeyBinding_FormatKeys(single,name,(uint32_t)std::size(name));
      std::copy_n(row->keys,length,candidate);
      candidateLength = length;
      if (length != 0) {
        for (uint16_t separator : {u' ',u'/',u' '}) {
          candidate[candidateLength++] = separator;
        }
      }
      if (candidateLength + nameLength + 1 > std::size(candidate)) {
        break;
      }
      std::copy_n(name,nameLength,candidate + candidateLength);
      candidateLength += nameLength;
      candidate[candidateLength] = 0;
      if (length != 0 && FrontendKeyBindingsPage_TextWidth(candidate) > KEY_BINDINGS_PAGE_KEY_LINE_WIDTH &&
          rowCount < KEY_BINDINGS_PAGE_MAX_ROWS) {
        row->keys[length++] = u' ';
        row->keys[length++] = u'/';
        row->keys[length] = 0;
        keyWidth = std::max(keyWidth,FrontendKeyBindingsPage_TextWidth(row->keys));
        row = &s_keyBindingsPageRows[rowCount++];
        row->action[0] = 0;
        std::copy_n(name,nameLength + 1,row->keys);
        length = nameLength;
      }
      else {
        std::copy_n(candidate,candidateLength + 1,row->keys);
        length = candidateLength;
      }
    }
    keyWidth = std::max(keyWidth,FrontendKeyBindingsPage_TextWidth(row->keys));
  }
  *outKeyWidth = keyWidth;
  *outActionWidth = actionWidth;
  return rowCount;
}

void FrontendKeyBindingsAction_OpenPage(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = FrontendKeyBindingsPage_Root(sourceNode);
  FrontendUiImage *ui = FrontendUi_Image(frontendRoot);
  UiListControl *list = &ui->keyBindingsList;
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  int32_t keyWidth;
  int32_t actionWidth;
  uint32_t rowCount;

  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,UiLayoutContainerControl_AsPageStack(&ui->frontendPageStack));
  UiPageStack_SetActiveIndex(FRONTEND_DISPLAY_SUBPAGE_MORE,UiLayoutContainerControl_AsPageStack(&ui->displayPageStack));
  UiPageStack_SetActiveIndex
            (FRONTEND_SETTINGS_SUBPAGE_KEY_BINDINGS,UiLayoutContainerControl_AsPageStack(&ui->settingsSubPageStack));
  /* as the display and advanced settings pages: the menu room is not drawn behind it in the compact layout */
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags = &ui->menuRoomModelView.contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  rowCount = FrontendKeyBindingsPage_BuildRows(&keyWidth,&actionWidth);
  for (uint32_t index = 0; index < rowCount; index++) {
    s_keyBindingsPageRowSlots[index] = &s_keyBindingsPageRows[index];
  }
  list->columns[0].width = keyWidth + KEY_BINDINGS_PAGE_KEY_COLUMN_GAP;
  list->columns[0].rowTextOffset = (uint32_t)offsetof(KeyBindingsPageRow,keys);
  ui->keyBindingsList_column2[0].width = actionWidth + KEY_BINDINGS_PAGE_ACTION_COLUMN_GAP;
  ui->keyBindingsList_column2[0].rowTextOffset = (uint32_t)offsetof(KeyBindingsPageRow,action);
  ui->keyBindingsScroller.scrollOffsetX = 0;
  ui->keyBindingsScroller.scrollOffsetY = 0;
  UiPointerList_InitializeColumnLayout(rowCount,s_keyBindingsPageRowSlots,UiListControl_AsPointerList(list));
  /* read-only: no row is selected (the list is suppressed, so it never takes a click or the keyboard focus) */
  list->selectedRowSlot = nullptr;
  UiNode_InvalidateRoot(&ui->keyBindingsPage.base);
}
