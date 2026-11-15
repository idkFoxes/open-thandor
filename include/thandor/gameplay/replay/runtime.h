#pragma once
#include <cstdint>
// Always compiled; inert unless OPEN_THANDOR_RECORD or OPEN_THANDOR_REPLAY is set.
bool GameplayReplay_PrepareSession();
void GameplayReplay_PeriodicClockTick();
bool GameplayReplay_Playing();
bool GameplayReplay_RecordingRequested();
bool GameplayReplay_LoadingCheckpoint();
bool GameplayReplay_Active();
const std::uint16_t *GameplayReplay_CheckpointPath();
bool GameplayReplay_CaptureInitialCheckpoint();
void GameplayReplay_BeginCheckpointLoad();
void GameplayReplay_EndCheckpointLoad();
void GameplayReplay_ConfigureLoadedPlayer(std::uint32_t faction);
bool GameplayReplay_SessionStarted();
void GameplayReplay_ClosingWorld();
void GameplayReplay_Close();
// Called under the tick lock. Playback supplies one recorded step and suppresses networking.
bool GameplayReplay_BeforeStep();
void GameplayReplay_AfterStep();
// Tick lock and ready state remain owned by the existing game loop.
class GameplayReplayCommandGuard {
public:
  GameplayReplayCommandGuard(std::uint32_t code, std::uint32_t player, std::uint32_t p1, std::uint32_t p2,
                             std::uint32_t p3);
  ~GameplayReplayCommandGuard();
  bool execute() const { return execute_; }
  GameplayReplayCommandGuard(const GameplayReplayCommandGuard &) = delete;
  GameplayReplayCommandGuard &operator=(const GameplayReplayCommandGuard &) = delete;

private:
  bool execute_ = true, entered_ = false;
};
