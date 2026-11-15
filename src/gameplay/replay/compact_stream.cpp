#include <algorithm>
#include <cstring>
#include <limits>
#include <thandor/gameplay/replay/compact_stream.h>
namespace thandor::replay {
namespace {
using Bytes = std::vector<std::uint8_t>;
constexpr std::uint32_t MaxCheckpoint = 128u * 1024u * 1024u;
constexpr std::uint32_t MaxRecord = 256;
void put(Bytes &b, std::uint64_t v, unsigned n) {
  for (unsigned i = 0; i < n; ++i)
    b.push_back(std::uint8_t(v >> (i * 8)));
}
struct Cursor {
  const Bytes &b;
  std::size_t at = 0;
  bool valid = true;
  std::uint64_t get(unsigned n) {
    if (n > b.size() - at) {
      valid = false;
      return 0;
    }
    std::uint64_t v = 0;
    for (unsigned i = 0; i < n; ++i)
      v |= std::uint64_t(b[at++]) << (8 * i);
    return v;
  }
};
std::uint32_t crc(const Bytes &b) {
  std::uint32_t c = ~0u;
  for (auto x : b) {
    c ^= x;
    for (int i = 0; i < 8; ++i)
      c = (c >> 1) ^ (0xedb88320u & (0u - (c & 1)));
  }
  return ~c;
}
bool fail(std::string &e, const char *s) {
  e = s;
  return false;
}
bool write(std::ofstream &f, const Bytes &b) {
  f.write(reinterpret_cast<const char *>(b.data()), static_cast<std::streamsize>(b.size()));
  return bool(f);
}
bool read(std::ifstream &f, Bytes &b, std::size_t n) {
  b.resize(n);
  f.read(reinterpret_cast<char *>(b.data()), static_cast<std::streamsize>(n));
  return f.gcount() == static_cast<std::streamsize>(n);
}
bool writeBlock(std::ofstream &f, const Bytes &b) {
  Bytes h;
  put(h, b.size(), 4);
  put(h, crc(b), 4);
  return write(f, h) && write(f, b);
}
bool readBlock(std::ifstream &f, Bytes &b, std::uint32_t max, std::string &e) {
  Bytes h;
  if (!read(f, h, 8))
    return fail(e, "truncated replay block header (missing clean footer)");
  Cursor c{h};
  auto n = c.get(4), checksum = c.get(4);
  if (n > max)
    return fail(e, "replay block exceeds bound");
  if (!read(f, b, static_cast<std::size_t>(n)))
    return fail(e, "truncated replay block");
  if (crc(b) != checksum)
    return fail(e, "replay CRC mismatch");
  return true;
}
} // namespace
std::uint64_t hashBytes(std::uint64_t h, const void *data, std::size_t n) {
  auto p = static_cast<const std::uint8_t *>(data);
  while (n--) {
    h ^= *p++;
    h *= 1099511628211ull;
  }
  return h;
}
bool Writer::open(const std::filesystem::path &path, const Header &h, std::string &e) {
  e.clear();
  if (file_.is_open())
    return fail(e, "writer already open");
  if (h.build.empty() || h.build.size() > 128 || h.checkpoint.empty() || h.checkpoint.size() > MaxCheckpoint)
    return fail(e, "invalid replay header/checkpoint");
  if (std::filesystem::exists(path))
    return fail(e, "recording already exists; choose a new filename");
  file_.open(path, std::ios::binary | std::ios::out);
  if (!file_)
    return fail(e, "cannot create recording");
  Bytes b = {'O', 'T', 'R', 'P', 'L', 'Y', '1', 0};
  put(b, 1, 4);
  put(b, h.build.size(), 4);
  b.insert(b.end(), h.build.begin(), h.build.end());
  put(b, h.assetsHash, 8);
  put(b, h.initialHash, 8);
  for (auto w : h.initialContext)
    put(b, w, 4);
  previous_ = h.initialContext;
  events_ = 0;
  rolling_ = 14695981039346656037ull;
  finished_ = false;
  if (!writeBlock(file_, b) || !writeBlock(file_, h.checkpoint))
    return fail(e, "cannot write replay header");
  return true;
}
bool Writer::append(const Event &ev, std::string &e) {
  if (!file_.is_open() || finished_)
    return fail(e, "writer is not active");
  if (ev.kind != Kind::Command && ev.kind != Kind::Step)
    return fail(e, "invalid replay event kind");
  Bytes b;
  put(b, std::uint8_t(ev.kind), 1);
  std::uint32_t mask = 0;
  for (std::size_t i = 0; i < ContextWords; ++i)
    if (ev.context[i] != previous_[i])
      mask |= std::uint32_t(1u << i);
  put(b, mask, 4);
  for (std::size_t i = 0; i < ContextWords; ++i)
    if (mask & (1u << i))
      put(b, ev.context[i], 4);
  put(b, ev.beforeHash, 8);
  put(b, ev.afterHash, 8);
  if (ev.kind == Kind::Command)
    for (auto w : ev.command)
      put(b, w, 4);
  if (!writeBlock(file_, b))
    return fail(e, "replay write failed");
  previous_ = ev.context;
  rolling_ = hashBytes(rolling_, b.data(), b.size());
  ++events_;
  // Bounded std::filebuf; periodic flush limits loss without flushing every tick.
  if (events_ % 256 == 0) {
    file_.flush();
    if (!file_)
      return fail(e, "replay flush failed");
  }
  return true;
}
bool Writer::finish(std::uint64_t h, std::string &e) {
  if (!file_.is_open() || finished_)
    return fail(e, "writer is not active");
  Bytes b;
  put(b, std::uint8_t(Kind::End), 1);
  put(b, events_, 8);
  put(b, rolling_, 8);
  put(b, h, 8);
  if (!writeBlock(file_, b))
    return fail(e, "cannot write replay footer");
  file_.flush();
  if (!file_)
    return fail(e, "cannot flush replay footer");
  file_.close();
  finished_ = true;
  return true;
}
bool Reader::open(const std::filesystem::path &path, Header &h, std::string &e) {
  e.clear();
  file_.close();
  file_.clear();
  file_.open(path, std::ios::binary);
  if (!file_)
    return fail(e, "cannot open recording");
  Bytes b;
  if (!readBlock(file_, b, 512, e))
    return false;
  if (b.size() < 16 || std::memcmp(b.data(), "OTRPLY1\0", 8))
    return fail(e, "invalid replay magic");
  Cursor c{b};
  c.at = 8;
  if (c.get(4) != 1)
    return fail(e, "unsupported replay version");
  auto n = c.get(4);
  if (!n || n > 128 || n > b.size() - c.at)
    return fail(e, "invalid build identity");
  h = Header{};
  h.build.assign(reinterpret_cast<const char *>(b.data() + c.at), static_cast<std::size_t>(n));
  c.at += static_cast<std::size_t>(n);
  h.assetsHash = c.get(8);
  h.initialHash = c.get(8);
  for (auto &w : h.initialContext)
    w = static_cast<std::uint32_t>(c.get(4));
  if (!c.valid || c.at != b.size())
    return fail(e, "invalid replay header length");
  if (!readBlock(file_, h.checkpoint, MaxCheckpoint, e) || h.checkpoint.empty())
    return fail(e, "invalid replay checkpoint");
  previous_ = h.initialContext;
  events_ = 0;
  rolling_ = 14695981039346656037ull;
  finished_ = false;
  return true;
}
bool Reader::next(Event &ev, std::string &e) {
  e.clear();
  if (finished_)
    return fail(e, "replay already completed");
  Bytes b;
  if (!readBlock(file_, b, MaxRecord, e))
    return false;
  Cursor c{b};
  ev = Event{};
  ev.kind = static_cast<Kind>(c.get(1));
  if (ev.kind == Kind::End) {
    auto count = c.get(8), digest = c.get(8);
    ev.afterHash = c.get(8);
    if (!c.valid || c.at != b.size() || count != events_ || digest != rolling_)
      return fail(e, "invalid replay footer");
    if (file_.peek() != std::char_traits<char>::eof())
      return fail(e, "trailing data after replay footer");
    finished_ = true;
    return true;
  }
  if (ev.kind != Kind::Command && ev.kind != Kind::Step)
    return fail(e, "unknown mandatory replay event");
  auto mask = c.get(4);
  if (mask >> ContextWords)
    return fail(e, "invalid replay context mask");
  ev.context = previous_;
  for (std::size_t i = 0; i < ContextWords; ++i)
    if (mask & (1u << i))
      ev.context[i] = static_cast<std::uint32_t>(c.get(4));
  ev.beforeHash = c.get(8);
  ev.afterHash = c.get(8);
  if (ev.kind == Kind::Command)
    for (auto &w : ev.command)
      w = static_cast<std::uint32_t>(c.get(4));
  if (!c.valid || c.at != b.size())
    return fail(e, "invalid replay event length");
  previous_ = ev.context;
  ++events_;
  rolling_ = hashBytes(rolling_, b.data(), b.size());
  return true;
}
} // namespace thandor::replay
