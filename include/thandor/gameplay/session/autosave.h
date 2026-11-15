/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/autosave.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_SESSION_AUTOSAVE_H
#define THANDOR_GAMEPLAY_SESSION_AUTOSAVE_H

/* open-thandor: the autosave. A local game (skirmish, campaign, tutorial: every game the original lets the player
   save; never a network game) is saved every AUTOSAVE_INTERVAL_SECONDS of play to save\autosave.sve, always the same
   file, through the normal save writer (InGameSaveGame_WritePackage: built as autosave.tmp, the old autosave is
   replaced only by a complete package). "Play" is wall-clock time of the running game: the frames of the session loop
   while the game is neither paused nor over, each counted at most AUTOSAVE_MAX_FRAME_GAP_MS, so a pause, an open
   window (they pause the local game), the briefing, a stalled window or the loading screen add nothing, while the game speed does not
   matter - the save comes after a minute of the player's time whatever the speed. The save runs in the session loop
   between two frames (InGameRuntime_RunSessionUntilExit), never inside a simulation step or a UI action; it waits
   while a window, a movie or a placement is open and is not made once the local faction ended or the level is over.
   A failed save is logged and the game goes on. The list entry shows "Autosave - <level title>" with the date and time
   of the save in the list's date column (Autosave_PrepareCatalogRecord). */

#include <cstdint>
#include <thandor/assets/scenario/types.h>

/* the autosave interval of the setting, in seconds of play */
inline constexpr int AUTOSAVE_INTERVAL_SECONDS = 60;
/* a frame counts at most this long (a stalled frame is no play time) */
inline constexpr int AUTOSAVE_MAX_FRAME_GAP_MS = 250;

/* The user setting "Automatisch speichern" ([game] autosave in thandor.ini, settings dword PERSISTENT_SETTING_AUTOSAVE;
   default on). While "Original Thandor" is on (OT_FEATURE_AUTOSAVE) no autosave is made whatever this says; the
   setting keeps its value. Existing autosaves stay listed and loadable. */
bool Autosave_Enabled();

/* A session has started (new level, loaded game, next campaign level): play time counts from zero. */
void Autosave_SessionStarted();

/* Once per frame of the session loop, after the frame: counts the play time and writes the autosave when it is due
   and the game is at a safe point (see above). Reads game state only, apart from the save writer's own temporary
   pointer conversions (undone before it returns); no random number, no simulation step. */
void Autosave_AfterFrame();

/* For both save lists (frontend ScenarioCatalog_Rebuild, in-game InGameSaveGamePage_RebuildCatalog), after the title
   ids were made text ids: an autosave's record (autosaveMarker) gets its list text "Autosave - <level title in the
   current language>" (cut with an ellipsis to the 256-pixel name column; the date and time stand in the date column
   next to it, from the record's timestampText) in identifier and keeps its file name in autosaveFileLeaf. Other
   records are left as they are. */
void Autosave_PrepareCatalogRecord(ScenarioCatalogSaveRecord *record);

/* The file name (leaf) of a save list record: autosaveFileLeaf for a prepared autosave record, else the record's
   identifier, as in the original. */
uint16_t *Autosave_SaveRecordFileLeaf(void *record);

#endif /* THANDOR_GAMEPLAY_SESSION_AUTOSAVE_H */
