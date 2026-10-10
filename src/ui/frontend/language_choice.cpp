/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/language_choice.cpp
 * Project code (not in the original game)
 */

/* The language choice of the advanced settings page (include/thandor/ui/frontend/language_choice.h).

   The list is advancedLanguageLayer, a page-sized layer linked after advancedLanguageGroup (the page's last child)
   while the list is open: the page draws it last and hit-tests it first, so it covers the page like a modal
   drop-down. The layer itself draws nothing; a press that reaches it (beside the list) closes the list, and it
   keeps the keyboard focus while open (the rows are no focus targets), so Escape closes the list instead of leaving
   the page. Unlinked, the layer is neither drawn, hit nor visited by the keyboard focus walk. */

#include <thandor/ui/frontend/language_choice.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/bootstrap/runtime.h>
#include <thandor/assets/text/richtext.h>
#include <thandor/ui/frontend/display_settings.h>

#include <cstddef>

/* rows of the list: "Automatisch" and the five languages */
static constexpr int LANGUAGE_CHOICE_ROWS = 6;
/* the list's box (advancedLanguagePanel): template top offset, row height, and the room around the rows (frame
   and title of the titled box, the rows' top offset) */
static constexpr int32_t LANGUAGE_CHOICE_LIST_TOP = 14;
static constexpr int32_t LANGUAGE_CHOICE_ROW_HEIGHT = 24;
static constexpr int32_t LANGUAGE_CHOICE_LIST_ROOM = 14;

/* Module data. */

/* the language (telephone country code, TEXT_RESOURCE_LANGUAGE_AUTO for row 0) of each list row, and the rows in use
   (filled when the list opens) */
static uint32_t s_LanguageChoiceRowCodes[LANGUAGE_CHOICE_ROWS] = {};
static int s_LanguageChoiceRowCount = 0;

static UiNodeBase *LanguageChoice_Root(UiNodeBase *node)
{
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  return node;
}

static bool LanguageChoice_ListOpen(FrontendUiImage *ui)
{
  return ui->advancedLanguageGroup.base.nextSibling.get() == &ui->advancedLanguageLayer.root.base;
}

/* Appends the text of textId to label (length code units so far, capacity with the terminator) as plain text: the
   leading colour command of a choice text is dropped (the button draws in its own style). */
static size_t LanguageChoice_AppendText(uint16_t *label,size_t length,size_t capacity,TextResourceId textId)
{
  const uint16_t *text = TextResource_Resolve(textId);

  if ((text[0] & RICHTEXT_COMMAND_FLAG) != 0) {
    text++;
  }
  for (; (*text != 0) && (length + 1 < capacity); text++) {
    label[length++] = *text;
  }
  label[length] = 0;
  return length;
}

static size_t LanguageChoice_AppendAscii(uint16_t *label,size_t length,size_t capacity,const char *text)
{
  for (; (*text != '\0') && (length + 1 < capacity); text++) {
    label[length++] = static_cast<unsigned char>(*text);
  }
  label[length] = 0;
  return length;
}

void FrontendLanguageChoice_Refresh(FrontendUiImage *ui)
{
  uint16_t label[TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS];
  const uint32_t selected = TextResource_SelectedLanguage();
  TextResourceId nameId;
  size_t length = 0;
  int row;

  label[0] = 0;
  if (selected == TEXT_RESOURCE_LANGUAGE_AUTO) {
    /* "Automatisch (Deutsch)": the language Windows gave */
    length = LanguageChoice_AppendText(label,length,TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS,TEXT_ID_LANGUAGE_AUTO);
    nameId = TextResource_LanguageNameId(TextResource_ActiveLanguage());
    if (nameId != TEXT_RESOURCE_ID_NONE) {
      length = LanguageChoice_AppendAscii(label,length,TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS," (");
      length = LanguageChoice_AppendText(label,length,TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS,nameId);
      LanguageChoice_AppendAscii(label,length,TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS,")");
    }
  }
  else {
    /* a language without a name (another country code from thandor.ini): the one in use */
    nameId = TextResource_LanguageNameId(selected);
    if (nameId == TEXT_RESOURCE_ID_NONE) {
      nameId = TextResource_LanguageNameId(TextResource_ActiveLanguage());
    }
    if (nameId != TEXT_RESOURCE_ID_NONE) {
      LanguageChoice_AppendText(label,length,TEXT_ID_LANGUAGE_BUTTON_LABEL_UNITS,nameId);
    }
  }
  TextResource_SetLanguageButtonLabel(label);
  for (row = 0; row < LANGUAGE_CHOICE_ROWS; row++) {
    UiSelectableControl_SetSelected((row < s_LanguageChoiceRowCount) && (s_LanguageChoiceRowCodes[row] == selected),
                                    &ui->advancedLanguageChoices[row].selectable);
  }
  UiNode_InvalidateRoot(&ui->advancedLanguageGroup.base);
}

/* Fills the rows: "Automatisch", then the languages menue.str has a block for; the rows left over are suppressed
   (and so hidden, UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) and the list's box ends below the last row in use. */
static void LanguageChoice_FillRows(FrontendUiImage *ui)
{
  uint32_t languages[TEXT_RESOURCE_LANGUAGE_MAX];
  const int languageCount = TextResource_AvailableLanguages(languages,TEXT_RESOURCE_LANGUAGE_MAX);
  int row;

  s_LanguageChoiceRowCodes[0] = TEXT_RESOURCE_LANGUAGE_AUTO;
  ui->advancedLanguageChoices[0].textResourceId = TEXT_ID_LANGUAGE_AUTO;
  s_LanguageChoiceRowCount = 1;
  for (int index = 0; (index < languageCount) && (s_LanguageChoiceRowCount < LANGUAGE_CHOICE_ROWS); index++) {
    s_LanguageChoiceRowCodes[s_LanguageChoiceRowCount] = languages[index];
    ui->advancedLanguageChoices[s_LanguageChoiceRowCount].textResourceId = TextResource_LanguageNameId(languages[index]);
    s_LanguageChoiceRowCount++;
  }
  for (row = 0; row < LANGUAGE_CHOICE_ROWS; row++) {
    UiNodeBase *node = &ui->advancedLanguageChoices[row].selectable.base;

    if (row < s_LanguageChoiceRowCount) {
      node->nodeFlags = node->nodeFlags & ~UI_NODE_SUPPRESSED;
    }
    else {
      node->nodeFlags = node->nodeFlags | UI_NODE_SUPPRESSED;
      s_LanguageChoiceRowCodes[row] = TEXT_RESOURCE_LANGUAGE_AUTO;
    }
  }
  ui->advancedLanguagePanel.root.base.bottomOffset =
       LANGUAGE_CHOICE_LIST_TOP + LANGUAGE_CHOICE_LIST_ROOM + s_LanguageChoiceRowCount * LANGUAGE_CHOICE_ROW_HEIGHT;
}

void FrontendLanguageChoice_CloseList(FrontendUiImage *ui)
{
  UiNodeBase *layer = &ui->advancedLanguageLayer.root.base;
  UiNodeBase *node;

  if (!LanguageChoice_ListOpen(ui)) {
    return;
  }
  ui->advancedLanguageGroup.base.nextSibling = UI_NODE_NONE;
  /* the focus is on the layer (or, from a press, on a node of it): back to the button */
  for (node = g_UiKeyboardFocusNode; node != UI_NODE_NONE; node = node->parent) {
    if (node == layer) {
      UiKeyboardFocus_Set(&ui->advancedLanguageButton.selectable.base);
      break;
    }
  }
  UiNode_InvalidateRoot(&ui->advancedSettingsPage.base);
}

void FrontendLanguageAction_ToggleList(UiNodeBase *sourceNode)
{
  FrontendUiImage *ui = FrontendUi_Image(LanguageChoice_Root(sourceNode));

  if (LanguageChoice_ListOpen(ui)) {
    FrontendLanguageChoice_CloseList(ui);
    return;
  }
  LanguageChoice_FillRows(ui);
  FrontendLanguageChoice_Refresh(ui);
  ui->advancedLanguageGroup.base.nextSibling = &ui->advancedLanguageLayer.root.base;
  /* the layer missed the page's layouts while it was unlinked (and the list's height may have changed) */
  UiContainer_LayoutChildren(&ui->advancedSettingsPage.base);
  UiKeyboardFocus_Set(&ui->advancedLanguageLayer.root.base);
  UiNode_InvalidateRoot(&ui->advancedSettingsPage.base);
}

void FrontendLanguageAction_Select(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = LanguageChoice_Root(sourceNode);
  FrontendUiImage *ui = FrontendUi_Image(frontendRoot);

  for (int row = 0; row < s_LanguageChoiceRowCount; row++) {
    if (sourceNode == &ui->advancedLanguageChoices[row].selectable.base) {
      if (s_LanguageChoiceRowCodes[row] != TextResource_SelectedLanguage()) {
        Game_ApplyLanguage(s_LanguageChoiceRowCodes[row]);
      }
      break;
    }
  }
  FrontendLanguageChoice_CloseList(ui);
  /* the note of the page is copied text (UI scale note), the button label as well */
  FrontendAdvancedSettingsPage_Refresh(frontendRoot);
}

/* nonRightPress of g_FrontendLanguageListLayerVtable: a press beside the list closes it (through the action queue,
   as the button does). */
static void FrontendLanguageListLayer_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)
{
  (void)wheelDelta;
  (void)pointerY;
  (void)pointerX;
  UiActionQueue_Enqueue(FRONTEND_ACTION_LANGUAGE_LIST,control);
}

/* keyboardEvent of g_FrontendLanguageListLayerVtable: Escape closes the list; other keys go on as usual. Returns
   false when the key is taken. */
static bool FrontendLanguageListLayer_KeyboardEvent
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)
{
  if (keyCode == KEYBOARD_KEY_CODE_ESCAPE) {
    UiActionQueue_Enqueue(FRONTEND_ACTION_LANGUAGE_LIST,control);
    return false;
  }
  return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,control);
}

UiNodeVtable g_FrontendLanguageListLayerVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren), /* no root flags: children */
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(FrontendLanguageListLayer_NonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(FrontendLanguageListLayer_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};
