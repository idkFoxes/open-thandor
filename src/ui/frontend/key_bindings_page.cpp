/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/key_bindings_page.cpp
 * Project code (not in the original game)
 */

#include <thandor/ui/frontend/key_bindings_page.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_bindings_panel.h>

/* The panel's box on the page, relative to the page's centre: the area of the former read-only list and its back
   button (608 x 284), below the page title. */
static constexpr UiPixelRect KEY_BINDINGS_PAGE_PANEL_RECT = {-304,-132,304,152};

/* The frontend root of any of its nodes. */
static UiNodeBase *FrontendKeyBindingsPage_Root(UiNodeBase *node)
{
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  return node;
}

/* The panel's close callback ("Zurueck", Esc): detaches the panel and returns to the options page (action 0x2010,
   as the other option pages' back buttons; context is the page node, so the handler reopens the options page). */
static void FrontendKeyBindingsPage_ClosePanel(void *context)
{
  KeyBindingsPanel_Detach();
  UiActionQueue_Enqueue(0x2010,context);
}

void FrontendKeyBindingsAction_OpenPage(UiNodeBase *sourceNode)
{
  UiNodeBase *frontendRoot = FrontendKeyBindingsPage_Root(sourceNode);
  FrontendUiImage *ui = FrontendUi_Image(frontendRoot);
  FrontendModelPointerContextFlags *menuRoomContextFlags;

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
  /* the editable panel as the page's last child (after the title); it takes the keyboard focus */
  KeyBindingsPanel_Attach(&ui->keyBindingsPage.base,KEY_BINDINGS_PAGE_PANEL_RECT,KEY_BINDINGS_PANEL_CENTERED,
                          &FrontendKeyBindingsPage_ClosePanel,&ui->keyBindingsPage.base);
  UiNode_InvalidateRoot(&ui->keyBindingsPage.base);
}
