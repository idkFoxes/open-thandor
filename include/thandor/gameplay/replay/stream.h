/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/replay/stream.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_GAMEPLAY_REPLAY_STREAM_H
#define THANDOR_GAMEPLAY_REPLAY_STREAM_H

/* Not in the original: the file format of a game recording (.otr, gameplay/replay/runtime.h). No pointers, no
   struct dumps: every value is written as little-endian bytes or a LEB128 varint.

   The file is a sequence of blocks: u32 payload length, u32 CRC-32 of the payload, payload.
     block 0  header: magic "OTREC1\0\0", u32 format version, the build id and build configuration, the asset
              fingerprint, the level name, the random seed the checkpoint was loaded with, the frontend player
              records, the initial context and the initial state hash
     block 1  checkpoint: the bytes of the save game the session was loaded from (an ordinary .sve package)
     then one block per event:
       u8 kind (RecordingEventKind; bit 0x80: the before hash equals the previous event's after hash and is left
       out), varint mask of the changed context words, per changed word a zigzag varint of (new - previous),
       [u64 before hash], u64 after hash, and for a command four u32 words (code << 8 | player, three payloads).
     the End event (kind End) closes the file: u64 event count, u64 rolling FNV-1a digest of the event payloads,
       u64 final state hash, u32 RecordingEndReason.
   A file without the End event (the game crashed or was killed) is valid up to its last complete block; the
   reader reports where it ends. */

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace thandor::replay {

/* The context words: what a step depends on besides the world itself, restored before every event in playback.
   0 simulation tick, 1 network tick counter, 2 step ticks (game speed), 3 presentation tick, 4 game speed Q8,
   5 UI command runtime flags, 6 primary seed, 7 secondary seed, 8 secondary stream selected, 9 local player id,
   10 active faction, 11..24 the camera (WorldMotionState). */
inline constexpr std::size_t CONTEXT_WORDS = 25;
using Context = std::array<std::uint32_t, CONTEXT_WORDS>;

enum class RecordingEventKind : std::uint8_t { Command = 1, Step = 2, Save = 3, End = 4 };

enum class RecordingEndReason : std::uint32_t {
  SessionEnded = 0,   /* the session ended (left, lost, won, loaded another game, next campaign level) */
  RecorderError = 1,  /* the recorder stopped (a write failed, an unrecordable command); the game went on */
};

inline constexpr std::uint32_t MAX_FRONTEND_PLAYERS = 8;

struct FrontendPlayer {
  std::uint32_t playerRuntimeId;
  std::uint32_t factionAssignmentIndex;
};

struct Header {
  std::string buildId;     /* version + git commit (+ dirty hash): playback refuses another one */
  std::string buildConfig; /* compiler, build type, developer tools: only reported */
  std::string levelName;
  std::string startedAt;   /* local time "YYYY-MM-DD HH:MM:SS" */
  std::uint64_t assetsHash = 0;
  std::uint32_t assetFileCount = 0;
  std::uint32_t loadSeed = 0;
  std::uint32_t pathingMode = 0;
  std::uint32_t sessionFlags = 0; /* bit 0: the session started a new level (its intro notifications) */
  std::vector<FrontendPlayer> frontendPlayers;
  Context initialContext{};
  std::uint64_t initialHash = 0;
};

struct Event {
  RecordingEventKind kind = RecordingEventKind::Step;
  Context context{};
  std::uint64_t beforeHash = 0;
  std::uint64_t afterHash = 0;
  std::array<std::uint32_t, 4> command{}; /* code << 8 | player, payload1..3 */
  /* End only */
  std::uint64_t eventCount = 0;
  std::uint64_t digest = 0;
  RecordingEndReason endReason = RecordingEndReason::SessionEnded;
};

/* Appends to a recording; hands its bytes to the operating system after every step event (WriteFile, no
   FlushFileBuffers), so a crash or a killed process loses at most the commands since the last step. */
class Writer {
public:
  Writer() = default;
  ~Writer();
  Writer(const Writer &) = delete;
  Writer &operator=(const Writer &) = delete;

  /* Creates path (fails when it exists) and writes the header and the checkpoint. */
  bool Open(const std::wstring &path, const Header &header, const std::vector<std::uint8_t> &checkpoint,
            std::string &error);
  bool Append(const Event &event, std::string &error);
  /* Writes the End event and closes the file. */
  bool Finish(std::uint64_t finalHash, RecordingEndReason reason, std::string &error);
  /* Writes what is buffered and closes without an End event. */
  void Abandon();
  bool IsOpen() const { return file_ != nullptr; }
  std::uint64_t EventCount() const { return events_; }
  std::uint64_t BytesWritten() const { return written_; }

private:
  bool WriteBlock(const std::vector<std::uint8_t> &payload, std::string &error);
  bool FlushPending(std::string &error);

  void *file_ = nullptr; /* HANDLE */
  std::vector<std::uint8_t> pending_;
  Context previous_{};
  std::uint64_t previousAfterHash_ = 0;
  std::uint64_t events_ = 0;
  std::uint64_t digest_ = 0;
  std::uint64_t written_ = 0;
};

/* Reads a whole recording into memory and hands out its events in order. */
class Reader {
public:
  enum class NextResult { Event, End, Truncated, Damaged };

  bool Open(const std::wstring &path, Header &header, std::vector<std::uint8_t> &checkpoint, std::string &error);
  /* Event: *event is the next event; End: the End event (in *event, its checks passed); Truncated: the file ends
     without an End event (a crash recording), error says where; Damaged: a block is damaged or malformed. */
  NextResult Next(Event &event, std::string &error);
  std::uint64_t EventCount() const { return events_; }

private:
  std::vector<std::uint8_t> bytes_;
  std::size_t at_ = 0;
  Context previous_{};
  std::uint64_t previousAfterHash_ = 0;
  std::uint64_t events_ = 0;
  std::uint64_t digest_ = 0;
};

std::uint32_t Crc32(const std::uint8_t *data, std::size_t size);

} // namespace thandor::replay

#endif /* THANDOR_GAMEPLAY_REPLAY_STREAM_H */
