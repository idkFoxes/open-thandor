/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/key_bindings_page.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H
#define THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H

#include <thandor/ui/controls/types.h>

/* The key binding page of the options menu ("Tastenbelegung", frontend only): the page title and the editable key
   binding panel (ui/core/key_bindings_panel.h), the frontend host of the panel. The page is settingsSubPageStack
   page 1 in displayPageStack page 1 (FrontendUiImage keyBindingsPage); the panel's "Zurueck" (or Esc) detaches it
   and returns to the options page through action 0x2010, as the other option pages' back buttons. */

/* Handler of FRONTEND_ACTION_OPEN_KEY_BINDINGS (the options page's "Tastenbelegung" button): shows the page and
   attaches the panel (rows in the current language, scrolled to the top, keyboard focus on its list). */
void FrontendKeyBindingsAction_OpenPage(UiNodeBase *sourceNode);

#endif /* THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H */
