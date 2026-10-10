/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/key_bindings_page.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H
#define THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H

#include <thandor/ui/controls/types.h>

/* The read-only key binding page of the options menu ("Tastenbelegung", frontend only): the rows of
   KeyBindings_Table (ui/core/key_bindings.h) as a scrollable two-column list (keys | action), each section headed by
   its title in the selected text style. The page is settingsSubPageStack page 1 in displayPageStack page 1
   (FrontendUiImage keyBindings*); its back button is action 0x2010 as on the other option pages. */

/* Handler of FRONTEND_ACTION_OPEN_KEY_BINDINGS (the options page's "Tastenbelegung" button): builds the rows in the
   current language of the key binding texts and shows the page scrolled to the top. */
void FrontendKeyBindingsAction_OpenPage(UiNodeBase *sourceNode);

#endif /* THANDOR_UI_FRONTEND_KEY_BINDINGS_PAGE_H */
