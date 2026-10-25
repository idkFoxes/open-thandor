/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/objective_overlay.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_INGAME_OBJECTIVE_OVERLAY_H
#define THANDOR_UI_INGAME_OBJECTIVE_OVERLAY_H

#include <stdint.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>

/* The objective overlay of the in-game UI (docs/plans/objective_overlay.md section 4): a small box at the top left
   of the world view, below the message lines and the faction status lines, with at most two lines - "Ziel:" (in the
   highlight colour) and the objective, then an optional progress line aligned with the objective text. Dark
   half-transparent background and the thin button frame of win.gfx; a line wider than the box may be is cut with an
   ellipsis. A left click on it opens the mission help like F1; the world view does not get that click.

   The box is not part of the in-game template (InGameUiImage / InGameRuntimeRoot stay as in the original): it is one
   node in module storage with its own vtable (g_InGameObjectiveOverlayVtable), linked into the running in-game root
   as the sibling right after the world view, so it is drawn above the world view and its texts and below the game
   windows, the message lines and the side panel, and it is hit-tested before the world view. The node covers the
   world view area; its draw and hit test use the box only.

   Shown only while all of these hold: "Original Thandor" off (OtFeature_Active(OT_FEATURE_OBJECTIVE_OVERLAY),
   core/settings/original_mode.h), ObjectiveOverlay_SetVisible(true), ObjectiveOverlay_Enabled(), a non-empty
   line 1, no game menu page (game menu, quit, save, graphics, sound) and no mission help open, no end movie, and the
   world view texts not hidden (UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS). Purely local display: no simulation,
   network or savegame state.

   All functions run on the UI thread (the game loop). */

/* Sets the overlay's lines (plain UTF-16, zero-terminated; copied, so the caller's buffers may change afterwards):
   line1 the objective (empty or nullptr: no objective, the overlay hides), line2 the progress or nullptr / empty
   for none. Control characters become spaces, code units of 0x8000 and above (rich-text commands) '?'; each line
   is kept to OBJECTIVE_OVERLAY_LINE_UNITS - 1 code units. Redraws only when a line changed. */
void ObjectiveOverlay_SetLines(const uint16_t *line1, const uint16_t *line2);

/* Shows or hides the overlay on top of the other conditions (for the update of package O5, e.g. the map editor).
   Every session starts with true; without lines the overlay stays hidden anyway. */
void ObjectiveOverlay_SetVisible(bool visible);

/* The user setting "Ziel anzeigen" ([game] objective_overlay in thandor.ini, settings dword
   PERSISTENT_SETTING_OBJECTIVE_OVERLAY; default on): false hides the overlay. */
bool ObjectiveOverlay_Enabled();

/* Switches the setting "Ziel anzeigen" (saved with the next settings flush) and redraws a running game's overlay;
   returns the new state (true: shown). While "Original Thandor" is on it does nothing and returns false. For the key "Ziel ein/aus" (key binding action objective_toggle, added when
   the changeable key bindings of experimental/keybinding-edit are merged; plan O5). */
bool ObjectiveOverlay_ToggleSetting();

/* The objective update (plan O5), called once per frame of the in-game UI (InGameUiRoot_UpdateFrame, on the UI
   thread, outside the simulation step): builds the local faction's objective (MissionObjective_Build) for the
   running level after a new root was attached (new, loaded, network game, next campaign mission), when the level's
   condition storage or the local faction changed and after a language switch (Game_LanguageGeneration); about twice
   a second (wall clock) it formats the progress line (MissionObjective_FormatProgress) and sets both lines
   (ObjectiveOverlay_SetLines redraws only on a change). Hides the overlay while the map editor is active. Reads game
   state only: no simulation, random number, arena, network or savegame effect. Does nothing while the setting is
   off (and with the developer test aid OPEN_THANDOR_OBJECTIVE_SAMPLE). */
void ObjectiveOverlay_Update();

/* Code units kept per line, terminator included. */
inline constexpr int OBJECTIVE_OVERLAY_LINE_UNITS = 128;

/* Links the overlay node into a newly created in-game root (after UiRootStack_Push relocated and laid out its
   tree) and lays it out over the world view area; InGameSession_CreateRoot calls it. The lines are cleared (a new
   session starts without objective) and SetVisible is reset to true. Developer tools: OPEN_THANDOR_OBJECTIVE_SAMPLE=1
   sets two sample lines, =long a line too wide for the box (truncation), =1line only line 1. */
void ObjectiveOverlay_AttachToRoot(InGameRuntimeRoot *root);

/* Unlinks the overlay node from the in-game root before that root is freed
   (InGameRuntime_ShutdownAndReleaseResources) and clears the lines. Does nothing when it is not linked to root. */
void ObjectiveOverlay_DetachFromRoot(InGameRuntimeRoot *root);

/* The overlay node's vtable (drawClipped, hitTest, press/release; the other slots are the plain node defaults). */
extern UiNodeVtable g_InGameObjectiveOverlayVtable;

#endif /* THANDOR_UI_INGAME_OBJECTIVE_OVERLAY_H */
