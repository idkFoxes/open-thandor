/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/language_choice.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_LANGUAGE_CHOICE_H
#define THANDOR_UI_FRONTEND_LANGUAGE_CHOICE_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>

/* The language choice of the advanced settings page ("Sprache:", frontend only): a drop-down list built from the
   original controls (FrontendUiImage advancedLanguage*). The button shows the chosen language ("Automatisch
   (Deutsch)" for the Windows language); it opens the list: "Automatisch" and every language menue.str has a block
   for (TextResource_AvailableLanguages), each named in its own language. A row switches the language at once
   (Game_ApplyLanguage, saved as [game] language) and closes the list; a press beside the list, Escape, or leaving
   the page closes it as well. */

/* The page-sized layer of the open list (advancedLanguageLayer): draws its children only, a press on it (beside
   the list) or Escape while it has the keyboard focus queues FRONTEND_ACTION_LANGUAGE_LIST (closes the list). */
extern UiNodeVtable g_FrontendLanguageListLayerVtable;

/* Shows the chosen language on the button (and the selected row of the list). Part of the advanced settings page's
   refresh (FrontendAdvancedSettingsPage_Refresh). */
void FrontendLanguageChoice_Refresh(FrontendUiImage *ui);

/* Closes the list (unlinks the layer from the page) and gives the keyboard focus back to the button when the list
   had it. The advanced settings page's opening calls it, so a list left open with the page is closed again. */
void FrontendLanguageChoice_CloseList(FrontendUiImage *ui);

/* Handler of FRONTEND_ACTION_LANGUAGE_LIST (the language button, the layer): opens the list when it is closed, else
   closes it. */
void FrontendLanguageAction_ToggleList(UiNodeBase *sourceNode);

/* Handler of FRONTEND_ACTION_LANGUAGE_SELECT (a list row): switches to the row's language, refreshes the page and
   closes the list. */
void FrontendLanguageAction_Select(UiNodeBase *sourceNode);

#endif /* THANDOR_UI_FRONTEND_LANGUAGE_CHOICE_H */
