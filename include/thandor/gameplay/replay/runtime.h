/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/replay/runtime.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_REPLAY_RUNTIME_H
#define THANDOR_GAMEPLAY_REPLAY_RUNTIME_H

#include <stdint.h>

/* Not in the original: the game recorder, a diagnostic tool (docs/BUILDING.md, "Game recordings"). It records a
   single player session into one .otr file (gameplay/replay/stream.h) - a checkpoint save game the session is
   reloaded from at its start, then every player command, every manual save and every simulation step with the
   context it ran in (ticks, game speed, UI flags, random seeds, camera) and state hashes before and after - and
   plays such a file back: the session is loaded from the checkpoint, live commands are blocked, the recorded ones
   are executed between the same steps, and the playback stops at the first state hash that differs.

   Recording: the "Erweitert" checkbox "Partien fuer Fehlerberichte aufzeichnen" ([game] record_games, off by
   default and off in "Original Thandor") writes recordings\<date>_<time>_<level>.otr next to thandor.exe;
   OPEN_THANDOR_RECORD=<path> (every build) records to that file instead and wins over the option, OPEN_THANDOR_RECORD=0
   switches recording off. Playback: OPEN_THANDOR_REPLAY=<path> or -REPLAY="<path>" on the command line; then the
   next local session started (any level) is replaced by the recording.

   Inactive (no recording, no playback) the hooks below only test g_GameRecorderActive / g_GameRecorderLoading, two
   plain booleans of the main thread; nothing runs on the timer thread. */

extern bool g_GameRecorderActive;  /* a session is recorded or played back (from its start to its end) */
extern bool g_GameRecorderLoading; /* the checkpoint is being loaded: no simulation steps */

inline bool GameRecorder_Active()
{
  return g_GameRecorderActive;
}

/* --- session lifecycle (gameplay/session/startup.cpp) --- */

/* InGameRuntime_RunSessionUntilExit, before the session is initialised: decides whether this session is recorded
   or played back. Never fails the session: a recording that cannot start is logged and the game runs unrecorded;
   a playback that cannot start is logged and the session starts as chosen. */
void GameRecorder_PrepareSession(bool loadExistingSession,const uint16_t *levelPathUtf16);
/* Playback: the session is to be loaded from GameRecorder_CheckpointPath instead of what the player chose. */
bool GameRecorder_LoadsCheckpointInstead();
/* Recording: the session that just started is to be saved and loaded again from the checkpoint. */
bool GameRecorder_ReloadsFromCheckpoint();
/* The checkpoint save game (a temporary file). */
uint16_t *GameRecorder_CheckpointPath();
/* Recording: saves the started session as the checkpoint; false (recording dropped, logged) when it fails. */
bool GameRecorder_CaptureCheckpoint();
/* Around InGameRuntime_InitializeLoadedSession of the checkpoint: seeds the random streams the same way in recording
   and playback and holds the simulation steps (g_GameRecorderLoading). */
void GameRecorder_BeginCheckpointLoad();
void GameRecorder_EndCheckpointLoad();
/* After the session is initialised (and reloaded), before its first frame: opens the recording and writes the header
   and checkpoint, or (playback) checks the loaded state against the recording. */
void GameRecorder_SessionStarted();
/* InGameRuntime_ShutdownAndReleaseResources, while the world still exists: ends the recording with its End event, or
   (playback) checks for it. */
void GameRecorder_SessionEnding();
/* InGameRuntime_RunSessionUntilExit, on every way out: closes files, removes the temporary checkpoint and
   restores the random stream selection. */
void GameRecorder_SessionClosed();

/* --- simulation step (gameplay/session/tick.cpp, under the tick lock) --- */

/* Before a due step: records the step's context (recording) or executes the recorded events up to the next step
   and restores its context (playback). False: skip this step (the tick lock is released by the caller). */
bool GameRecorder_BeforeStep();
void GameRecorder_AfterStep();
/* Playback with OPEN_THANDOR_REPLAY_FAST=1: steps do not wait for the 80 Hz timer. */
bool GameRecorder_StepsUnpaced();

/* --- commands (network/protocol/commands.h, InGameCommand_Issue / InGameCommand_IssueHandler) --- */

/* One local in-game command: recorded (recording) or, coming from the live UI, blocked (playback; only leaving the
   game passes). code is the handler's in-game command code (0xFFFFFFFF: none, which stops a recording). */
class GameRecorderCommandScope {
public:
  GameRecorderCommandScope(uint32_t code,uint32_t player,uint32_t payload1,uint32_t payload2,uint32_t payload3);
  ~GameRecorderCommandScope();
  bool Execute() const { return execute_; }
  GameRecorderCommandScope(const GameRecorderCommandScope &) = delete;
  GameRecorderCommandScope &operator=(const GameRecorderCommandScope &) = delete;

private:
  bool execute_ = true;
  bool recorded_ = false;
};

/* --- manual saves (gameplay/session/savegame.cpp, InGameSaveGame_WritePackage) --- */

/* Around a save written while a session is recorded: writing a save changes the game state a little (pending army
   assets go back to their faction, the camera becomes the player start), so a manual save is an event of its own
   that the playback repeats into a temporary file. The autosave puts the state back afterwards and is no event. */
class GameRecorderSaveScope {
public:
  explicit GameRecorderSaveScope(bool autosave);
  ~GameRecorderSaveScope();
  GameRecorderSaveScope(const GameRecorderSaveScope &) = delete;
  GameRecorderSaveScope &operator=(const GameRecorderSaveScope &) = delete;

private:
  bool recorded_ = false;
};
/* Playback: false while live saves must not be written (the autosave of a played back session). */
bool GameRecorder_AutosaveAllowed();

#endif /* THANDOR_GAMEPLAY_REPLAY_RUNTIME_H */
