#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
namespace custom_maps {
struct Project {
  std::filesystem::path file;
  std::string title, author, level_entry, field_entry, string_entry;
  uint32_t width{}, height{}, human_factions{}, total_factions{}, planet{};
  uint32_t default_human_faction{1};
  std::array<bool, 8> human_allowed{};
  std::vector<std::string> texts;
  std::map<std::string, std::vector<std::byte>> assets;
  static Project load(const std::filesystem::path &file);
};
std::vector<std::filesystem::path>
discover(const std::filesystem::path &directory);
std::string normalized_path(std::string path);
} // namespace custom_maps
