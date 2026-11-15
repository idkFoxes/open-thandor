#pragma once
// Portable on-disk protocol. No native pointers, struct dumps or SDK dependencies.
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
namespace thandor::replay {
constexpr std::size_t ContextWords = 26;
using Context = std::array<std::uint32_t, ContextWords>;
struct Header {
  std::string build;
  std::uint64_t assetsHash = 0, initialHash = 0;
  Context initialContext{};
  std::vector<std::uint8_t> checkpoint;
};
enum class Kind : std::uint8_t { Command = 1, Step = 2, End = 3 };
struct Event {
  Kind kind = Kind::Step;
  Context context{};
  std::array<std::uint32_t, 4> command{}; // packed command/player, three payloads
  std::uint64_t beforeHash = 0, afterHash = 0;
};
std::uint64_t hashBytes(std::uint64_t hash, const void *data, std::size_t size);
class Writer {
public:
  bool open(const std::filesystem::path &, const Header &, std::string &error);
  bool append(const Event &, std::string &error);
  bool finish(std::uint64_t finalHash, std::string &error);
  bool active() const { return file_.is_open(); }

private:
  std::ofstream file_;
  Context previous_{};
  std::uint64_t events_ = 0, rolling_ = 14695981039346656037ull;
  bool finished_ = false;
};
class Reader {
public:
  bool open(const std::filesystem::path &, Header &, std::string &error);
  // EOF without an explicit End is an error; clean completion is an End event.
  bool next(Event &, std::string &error);

private:
  std::ifstream file_;
  Context previous_{};
  std::uint64_t events_ = 0, rolling_ = 14695981039346656037ull;
  bool finished_ = false;
};
} // namespace thandor::replay
