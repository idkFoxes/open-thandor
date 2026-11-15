/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/key_bindings_overlay.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_INGAME_KEY_BINDINGS_OVERLAY_H
#define THANDOR_UI_INGAME_KEY_BINDINGS_OVERLAY_H

#include <thandor/ui/controls/types.h>

/* The in-game host of the key binding panel (ui/core/key_bindings_panel.h, docs/plans/key_rebinding.md P5): the
   help window's tab "Tastatur" opens the panel as a modal overlay. A transparent layer (module storage) covering
   the whole in-game root is linked as the root's last child (drawn last, hit first, so no press reaches the world
   view, the side panel or the help window below it); the panel, with window background, frame and title, sits in
   it centred over the world view, as large as the frontend page's panel when the world view is large enough
   (at least KEY_BINDINGS_PANEL_MIN_WIDTH x KEY_BINDINGS_PANEL_MIN_HEIGHT). "Zurueck" or Esc closes the overlay; the
   help window stays open on its "Auftrag" tab (a local game stays paused by it). While the overlay is open the
   world view's key commands and the in-game hotkeys take no key (InGameKeyBindingsOverlay_IsOpen), and while the
   panel captures a key the held camera keys are not applied. */

/* Opens the overlay over the in-game root of sourceNode (any node of the in-game tree). */
void InGameKeyBindingsOverlay_Open(UiNodeBase *sourceNode);

/* Closes the overlay: detaches the panel and unlinks the layer. Nothing happens when it is not open. Called on
   "Zurueck" and before the in-game root is freed. */
void InGameKeyBindingsOverlay_Close();

bool InGameKeyBindingsOverlay_IsOpen();

#endif /* THANDOR_UI_INGAME_KEY_BINDINGS_OVERLAY_H */
