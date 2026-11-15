/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/replay/stream.cpp
 * Project code (not in the original game)
 */

#include <thandor/gameplay/replay/stream.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

/* Not in the original: the .otr format (gameplay/replay/stream.h). The block framing (length + CRC-32) and the
   delta-coded context follow the compact replay stream of an open-thandor fork based on 1.0.7; the varint coding,
   the omitted before hashes, the incremental writes and the reading of a file without its End event are this
   version's. */

namespace thandor::replay {

namespace {

using Bytes = std::vector<std::uint8_t>;

constexpr char MAGIC[8] = {'O', 'T', 'R', 'E', 'C', '1', 0, 0};
constexpr std::uint32_t FORMAT_VERSION = 1;
constexpr std::uint32_t MAX_HEADER_BYTES = 4096;
constexpr std::uint32_t MAX_CHECKPOINT_BYTES = 256u * 1024u * 1024u;
constexpr std::uint32_t MAX_EVENT_BYTES = 256;
constexpr std::uint64_t MAX_FILE_BYTES = 2ull * 1024u * 1024u * 1024u;
constexpr std::uint8_t KIND_BEFORE_IS_PREVIOUS_AFTER = 0x80;
constexpr std::uint64_t DIGEST_SEED = 0xcbf29ce484222325ull;

std::uint64_t Digest(std::uint64_t hash, const std::uint8_t *data, std::size_t size)
{
  while (size-- != 0) {
    hash ^= *data++;
    hash *= 0x100000001b3ull;
  }
  return hash;
}

void PutFixed(Bytes &out, std::uint64_t value, unsigned byteCount)
{
  for (unsigned index = 0; index < byteCount; index++) {
    out.push_back(static_cast<std::uint8_t>(value >> (index * 8)));
  }
}

void PutVarint(Bytes &out, std::uint64_t value)
{
  while (value >= 0x80) {
    out.push_back(static_cast<std::uint8_t>(value | 0x80));
    value >>= 7;
  }
  out.push_back(static_cast<std::uint8_t>(value));
}

void PutString(Bytes &out, const std::string &text)
{
  PutVarint(out, text.size());
  out.insert(out.end(), text.begin(), text.end());
}

/* zigzag of a 32-bit difference (wrapping): small steps either way stay one or two bytes */
std::uint32_t Zigzag(std::uint32_t delta)
{
  const std::int32_t signedDelta = static_cast<std::int32_t>(delta);
  return (static_cast<std::uint32_t>(signedDelta) << 1) ^ static_cast<std::uint32_t>(signedDelta >> 31);
}

std::uint32_t Unzigzag(std::uint32_t value)
{
  return (value >> 1) ^ (0u - (value & 1u));
}

struct Cursor {
  const std::uint8_t *data;
  std::size_t size;
  std::size_t at = 0;
  bool valid = true;

  std::uint64_t Fixed(unsigned byteCount)
  {
    std::uint64_t value = 0;
    if (byteCount > size - at) {
      valid = false;
      at = size;
      return 0;
    }
    for (unsigned index = 0; index < byteCount; index++) {
      value |= static_cast<std::uint64_t>(data[at++]) << (index * 8);
    }
    return value;
  }

  std::uint64_t Varint()
  {
    std::uint64_t value = 0;
    for (unsigned shift = 0; shift < 64; shift += 7) {
      if (at >= size) {
        valid = false;
        return 0;
      }
      const std::uint8_t byte = data[at++];
      value |= static_cast<std::uint64_t>(byte & 0x7F) << shift;
      if ((byte & 0x80) == 0) {
        return value;
      }
    }
    valid = false;
    return 0;
  }

  std::string String(std::size_t maxBytes)
  {
    const std::uint64_t length = Varint();
    if (!valid || length > maxBytes || length > size - at) {
      valid = false;
      return {};
    }
    std::string text(reinterpret_cast<const char *>(data + at), static_cast<std::size_t>(length));
    at += static_cast<std::size_t>(length);
    return text;
  }

  bool AtEnd() const { return valid && at == size; }
};

bool Fail(std::string &error, const char *text)
{
  error = text;
  return false;
}

void PutContextDelta(Bytes &out, const Context &context, const Context &previous)
{
  std::uint32_t mask = 0;
  for (std::size_t index = 0; index < CONTEXT_WORDS; index++) {
    if (context[index] != previous[index]) {
      mask |= 1u << index;
    }
  }
  PutVarint(out, mask);
  for (std::size_t index = 0; index < CONTEXT_WORDS; index++) {
    if ((mask >> index) & 1u) {
      PutVarint(out, Zigzag(context[index] - previous[index]));
    }
  }
}

bool GetContextDelta(Cursor &cursor, Context &context, const Context &previous)
{
  const std::uint64_t mask = cursor.Varint();
  if (!cursor.valid || (mask >> CONTEXT_WORDS) != 0) {
    return false;
  }
  context = previous;
  for (std::size_t index = 0; index < CONTEXT_WORDS; index++) {
    if ((mask >> index) & 1u) {
      const std::uint64_t value = cursor.Varint();
      if (value > 0xFFFFFFFFull) {
        return false;
      }
      context[index] = previous[index] + Unzigzag(static_cast<std::uint32_t>(value));
    }
  }
  return cursor.valid;
}

} // namespace

std::uint32_t Crc32(const std::uint8_t *data, std::size_t size)
{
  static std::uint32_t s_table[256];
  static bool s_tableReady;
  if (!s_tableReady) {
    for (std::uint32_t index = 0; index < 256; index++) {
      std::uint32_t value = index;
      for (int bit = 0; bit < 8; bit++) {
        value = (value >> 1) ^ (0xEDB88320u & (0u - (value & 1u)));
      }
      s_table[index] = value;
    }
    s_tableReady = true;
  }
  std::uint32_t crc = 0xFFFFFFFFu;
  for (std::size_t index = 0; index < size; index++) {
    crc = s_table[(crc ^ data[index]) & 0xFF] ^ (crc >> 8);
  }
  return ~crc;
}

/* ---- writer ---- */

Writer::~Writer()
{
  Abandon();
}

bool Writer::WriteBlock(const Bytes &payload, std::string &error)
{
  if (file_ == nullptr) {
    return Fail(error, "the recording is not open");
  }
  PutFixed(pending_, payload.size(), 4);
  PutFixed(pending_, Crc32(payload.data(), payload.size()), 4);
  pending_.insert(pending_.end(), payload.begin(), payload.end());
  return true;
}

bool Writer::FlushPending(std::string &error)
{
  std::size_t done = 0;
  while (done < pending_.size()) {
    DWORD wrote = 0;
    const DWORD chunk = static_cast<DWORD>(std::min<std::size_t>(pending_.size() - done, 1u << 24));
    if (!WriteFile(static_cast<HANDLE>(file_), pending_.data() + done, chunk, &wrote, nullptr) || wrote == 0) {
      pending_.erase(pending_.begin(), pending_.begin() + static_cast<std::ptrdiff_t>(done));
      return Fail(error, "writing the recording failed (disk full?)");
    }
    done += wrote;
  }
  written_ += pending_.size();
  pending_.clear();
  return true;
}

bool Writer::Open(const std::wstring &path, const Header &header, const Bytes &checkpoint, std::string &error)
{
  Bytes block;

  error.clear();
  if (file_ != nullptr) {
    return Fail(error, "a recording is already open");
  }
  if (checkpoint.empty() || checkpoint.size() > MAX_CHECKPOINT_BYTES) {
    return Fail(error, "the checkpoint is empty or too large");
  }
  HANDLE handle = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return Fail(error, GetLastError() == ERROR_FILE_EXISTS ? "the recording file exists already"
                                                           : "the recording file cannot be created");
  }
  file_ = handle;
  pending_.clear();
  events_ = 0;
  written_ = 0;
  digest_ = DIGEST_SEED;
  block.insert(block.end(), MAGIC, MAGIC + sizeof MAGIC);
  PutFixed(block, FORMAT_VERSION, 4);
  PutString(block, header.buildId);
  PutString(block, header.buildConfig);
  PutString(block, header.levelName);
  PutString(block, header.startedAt);
  PutFixed(block, header.assetsHash, 8);
  PutVarint(block, header.assetFileCount);
  PutFixed(block, header.loadSeed, 4);
  PutVarint(block, header.pathingMode);
  PutVarint(block, header.sessionFlags);
  PutVarint(block, header.frontendPlayers.size());
  for (const FrontendPlayer &player : header.frontendPlayers) {
    PutVarint(block, player.playerRuntimeId);
    PutVarint(block, player.factionAssignmentIndex);
  }
  for (std::uint32_t word : header.initialContext) {
    PutFixed(block, word, 4);
  }
  PutFixed(block, header.initialHash, 8);
  if (block.size() > MAX_HEADER_BYTES || header.frontendPlayers.size() > MAX_FRONTEND_PLAYERS) {
    Abandon();
    return Fail(error, "the recording header is too large");
  }
  previous_ = header.initialContext;
  previousAfterHash_ = header.initialHash;
  if (!WriteBlock(block, error) || !WriteBlock(checkpoint, error) || !FlushPending(error)) {
    Abandon();
    return false;
  }
  return true;
}

bool Writer::Append(const Event &event, std::string &error)
{
  Bytes block;
  const bool beforeOmitted = event.beforeHash == previousAfterHash_;

  if (file_ == nullptr) {
    return Fail(error, "the recording is not open");
  }
  if (event.kind != RecordingEventKind::Command && event.kind != RecordingEventKind::Step &&
      event.kind != RecordingEventKind::Save) {
    return Fail(error, "invalid event kind");
  }
  block.reserve(64);
  block.push_back(static_cast<std::uint8_t>(event.kind) | (beforeOmitted ? KIND_BEFORE_IS_PREVIOUS_AFTER : 0));
  PutContextDelta(block, event.context, previous_);
  if (!beforeOmitted) {
    PutFixed(block, event.beforeHash, 8);
  }
  PutFixed(block, event.afterHash, 8);
  if (event.kind == RecordingEventKind::Command) {
    for (std::uint32_t word : event.command) {
      PutFixed(block, word, 4);
    }
  }
  if (!WriteBlock(block, error)) {
    return false;
  }
  previous_ = event.context;
  previousAfterHash_ = event.afterHash;
  digest_ = Digest(digest_, block.data(), block.size());
  events_++;
  /* a step closes what happened since the last one: hand it to the operating system now */
  if (event.kind == RecordingEventKind::Step || event.kind == RecordingEventKind::Save) {
    return FlushPending(error);
  }
  return true;
}

bool Writer::Finish(std::uint64_t finalHash, RecordingEndReason reason, std::string &error)
{
  Bytes block;

  if (file_ == nullptr) {
    return Fail(error, "the recording is not open");
  }
  block.push_back(static_cast<std::uint8_t>(RecordingEventKind::End));
  PutFixed(block, events_, 8);
  PutFixed(block, digest_, 8);
  PutFixed(block, finalHash, 8);
  PutFixed(block, static_cast<std::uint32_t>(reason), 4);
  const bool ok = WriteBlock(block, error) && FlushPending(error);
  CloseHandle(static_cast<HANDLE>(file_));
  file_ = nullptr;
  return ok;
}

void Writer::Abandon()
{
  if (file_ == nullptr) {
    return;
  }
  std::string ignored;
  FlushPending(ignored);
  CloseHandle(static_cast<HANDLE>(file_));
  file_ = nullptr;
}

/* ---- reader ---- */

bool Reader::Open(const std::wstring &path, Header &header, Bytes &checkpoint, std::string &error)
{
  error.clear();
  bytes_.clear();
  at_ = 0;
  HANDLE handle = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return Fail(error, "the recording cannot be opened");
  }
  LARGE_INTEGER size;
  if (!GetFileSizeEx(handle, &size) || size.QuadPart < 16 || static_cast<std::uint64_t>(size.QuadPart) > MAX_FILE_BYTES) {
    CloseHandle(handle);
    return Fail(error, "the recording is empty or larger than 2 GB");
  }
  try {
    bytes_.resize(static_cast<std::size_t>(size.QuadPart));
  }
  catch (...) {
    CloseHandle(handle);
    return Fail(error, "no memory for the recording");
  }
  std::size_t done = 0;
  while (done < bytes_.size()) {
    DWORD got = 0;
    const DWORD chunk = static_cast<DWORD>(std::min<std::size_t>(bytes_.size() - done, 1u << 24));
    if (!ReadFile(handle, bytes_.data() + done, chunk, &got, nullptr) || got == 0) {
      break;
    }
    done += got;
  }
  CloseHandle(handle);
  bytes_.resize(done);

  /* header block */
  auto takeBlock = [this](std::uint32_t maxBytes, const std::uint8_t **payload, std::uint32_t *length) {
    if (bytes_.size() - at_ < 8) {
      return false;
    }
    Cursor frame{bytes_.data() + at_, 8};
    const auto blockLength = static_cast<std::uint32_t>(frame.Fixed(4));
    const auto crc = static_cast<std::uint32_t>(frame.Fixed(4));
    if (blockLength > maxBytes || blockLength > bytes_.size() - at_ - 8 ||
        Crc32(bytes_.data() + at_ + 8, blockLength) != crc) {
      return false;
    }
    *payload = bytes_.data() + at_ + 8;
    *length = blockLength;
    at_ += 8 + static_cast<std::size_t>(blockLength);
    return true;
  };
  const std::uint8_t *payload;
  std::uint32_t length;
  if (!takeBlock(MAX_HEADER_BYTES, &payload, &length) || length < sizeof MAGIC + 4 ||
      std::memcmp(payload, MAGIC, sizeof MAGIC) != 0) {
    return Fail(error, "not an open-thandor recording (.otr) or its header is damaged");
  }
  Cursor cursor{payload, length};
  cursor.at = sizeof MAGIC;
  if (cursor.Fixed(4) != FORMAT_VERSION) {
    return Fail(error, "the recording has another format version");
  }
  header = Header{};
  header.buildId = cursor.String(256);
  header.buildConfig = cursor.String(256);
  header.levelName = cursor.String(256);
  header.startedAt = cursor.String(64);
  header.assetsHash = cursor.Fixed(8);
  header.assetFileCount = static_cast<std::uint32_t>(cursor.Varint());
  header.loadSeed = static_cast<std::uint32_t>(cursor.Fixed(4));
  header.pathingMode = static_cast<std::uint32_t>(cursor.Varint());
  header.sessionFlags = static_cast<std::uint32_t>(cursor.Varint());
  const std::uint64_t playerCount = cursor.Varint();
  if (!cursor.valid || playerCount > MAX_FRONTEND_PLAYERS) {
    return Fail(error, "the recording header is damaged");
  }
  for (std::uint64_t index = 0; index < playerCount; index++) {
    FrontendPlayer player;
    player.playerRuntimeId = static_cast<std::uint32_t>(cursor.Varint());
    player.factionAssignmentIndex = static_cast<std::uint32_t>(cursor.Varint());
    header.frontendPlayers.push_back(player);
  }
  for (std::uint32_t &word : header.initialContext) {
    word = static_cast<std::uint32_t>(cursor.Fixed(4));
  }
  header.initialHash = cursor.Fixed(8);
  if (!cursor.AtEnd()) {
    return Fail(error, "the recording header is damaged");
  }
  if (!takeBlock(MAX_CHECKPOINT_BYTES, &payload, &length) || length == 0) {
    return Fail(error, "the recording's checkpoint is missing or damaged");
  }
  checkpoint.assign(payload, payload + length);
  previous_ = header.initialContext;
  previousAfterHash_ = header.initialHash;
  events_ = 0;
  digest_ = DIGEST_SEED;
  return true;
}

Reader::NextResult Reader::Next(Event &event, std::string &error)
{
  char text[160];

  error.clear();
  if (at_ >= bytes_.size()) {
    snprintf(text, sizeof text, "recording ends at event %llu (no clean end)",
             static_cast<unsigned long long>(events_));
    error = text;
    return NextResult::Truncated;
  }
  if (bytes_.size() - at_ < 8) {
    snprintf(text, sizeof text, "recording ends at event %llu (no clean end, %u bytes of a cut-off block)",
             static_cast<unsigned long long>(events_), static_cast<unsigned>(bytes_.size() - at_));
    error = text;
    return NextResult::Truncated;
  }
  Cursor frame{bytes_.data() + at_, 8};
  const auto length = static_cast<std::uint32_t>(frame.Fixed(4));
  const auto crc = static_cast<std::uint32_t>(frame.Fixed(4));
  if (length > MAX_EVENT_BYTES || length == 0) {
    snprintf(text, sizeof text, "recording damaged after event %llu (block length %u)",
             static_cast<unsigned long long>(events_), length);
    error = text;
    return NextResult::Damaged;
  }
  if (length > bytes_.size() - at_ - 8) {
    snprintf(text, sizeof text, "recording ends at event %llu (no clean end, last block cut off)",
             static_cast<unsigned long long>(events_));
    error = text;
    return NextResult::Truncated;
  }
  const std::uint8_t *payload = bytes_.data() + at_ + 8;
  if (Crc32(payload, length) != crc) {
    const bool last = at_ + 8 + length == bytes_.size();
    snprintf(text, sizeof text, last ? "recording ends at event %llu (no clean end, last block incomplete)"
                                     : "recording damaged after event %llu (checksum)",
             static_cast<unsigned long long>(events_));
    error = text;
    return last ? NextResult::Truncated : NextResult::Damaged;
  }
  at_ += 8 + static_cast<std::size_t>(length);
  Cursor cursor{payload, length};
  const auto kindByte = static_cast<std::uint8_t>(cursor.Fixed(1));
  event = Event{};
  event.kind = static_cast<RecordingEventKind>(kindByte & ~KIND_BEFORE_IS_PREVIOUS_AFTER);
  if (event.kind == RecordingEventKind::End) {
    event.eventCount = cursor.Fixed(8);
    event.digest = cursor.Fixed(8);
    event.afterHash = cursor.Fixed(8);
    event.endReason = static_cast<RecordingEndReason>(cursor.Fixed(4));
    if (!cursor.AtEnd() || event.eventCount != events_ || event.digest != digest_) {
      snprintf(text, sizeof text, "the End event after event %llu does not match the events before it",
               static_cast<unsigned long long>(events_));
      error = text;
      return NextResult::Damaged;
    }
    return NextResult::End;
  }
  if (event.kind != RecordingEventKind::Command && event.kind != RecordingEventKind::Step &&
      event.kind != RecordingEventKind::Save) {
    snprintf(text, sizeof text, "unknown event kind %u after event %llu", kindByte,
             static_cast<unsigned long long>(events_));
    error = text;
    return NextResult::Damaged;
  }
  if (!GetContextDelta(cursor, event.context, previous_)) {
    snprintf(text, sizeof text, "malformed event after event %llu", static_cast<unsigned long long>(events_));
    error = text;
    return NextResult::Damaged;
  }
  event.beforeHash = (kindByte & KIND_BEFORE_IS_PREVIOUS_AFTER) != 0 ? previousAfterHash_ : cursor.Fixed(8);
  event.afterHash = cursor.Fixed(8);
  if (event.kind == RecordingEventKind::Command) {
    for (std::uint32_t &word : event.command) {
      word = static_cast<std::uint32_t>(cursor.Fixed(4));
    }
  }
  if (!cursor.AtEnd()) {
    snprintf(text, sizeof text, "malformed event after event %llu", static_cast<unsigned long long>(events_));
    error = text;
    return NextResult::Damaged;
  }
  previous_ = event.context;
  previousAfterHash_ = event.afterHash;
  digest_ = Digest(digest_, payload, length);
  events_++;
  return NextResult::Event;
}

} // namespace thandor::replay
