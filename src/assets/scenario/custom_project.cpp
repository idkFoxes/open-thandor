#include "thandor/map_project_archive.hpp"
#include "thandor/text_resource.hpp"
#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <thandor/assets/scenario/custom_project.h>
namespace custom_maps {
using namespace thandor;
static void need(bool value, const char *reason) {
  if (!value)
    throw FormatError(reason);
}
std::string normalized_path(std::string path) {
  return normalize_project_path(path);
}

Project Project::load(const std::filesystem::path &file) {
  auto archive = MapProjectArchive::open(file);
  const auto meta = archive.manifest();
  auto edited_terrain = read_edited_terrain_images(archive, meta);
  Project p;
  p.file = file;
  p.level_entry = normalized_path(meta.at("level_entry"));
  p.field_entry = normalized_path(meta.at("field_entry"));
  if (meta.contains("string_entry"))
    p.string_entry = normalized_path(meta.at("string_entry"));
  need(p.level_entry.starts_with("level/") && p.level_entry.ends_with(".lev") &&
           p.field_entry.ends_with(".fld") &&
           (p.string_entry.empty() || p.string_entry.ends_with(".str")),
       "Invalid project entry types");
  if (meta.contains("scenario_entry")) {
    const auto &record = archive.require(meta.at("scenario_entry"));
    need(record.size() == 256, "Invalid scenario metadata");
    p.planet = read_u32(record, 0x50);
    need(p.planet <= 9, "Invalid planet index");
  }
  for (auto &[path, data] : archive.files) {
    if (path != kZipMapManifest && !path.starts_with("editor/") &&
        !(archive.zip && path.starts_with(kEditableTerrainDirectory)))
      p.assets.emplace(path, std::move(data));
  }
  auto &lev = p.assets.at(p.level_entry);
  auto &fld = p.assets.at(p.field_entry);
  need(lev.size() >= 0x340 && read_u32(lev, 0) == fourcc("lev") &&
           read_u32(lev, 4) == lev.size() && read_u32(lev, 12) == 0x70001,
       "Invalid project LEV");
  // Every LEV path table used by runtime must be a terminated in-image UTF-16
  // string.
  for (size_t at = 0xb0; at <= 0xd4; at += 4) {
    auto off = read_u32(lev, at);
    need(off >= 0x340 && off <= 0xffff && (off & 1) == 0 &&
             off + 10 < lev.size(),
         "Invalid LEV path offset");
    bool end = false;
    for (size_t i = off; i + 10 < lev.size(); i += 2)
      if (read_u16(lev, i) == 0) {
        end = true;
        break;
      }
    need(end, "Unterminated LEV path");
  }
  const auto fieldbase = read_utf16le_z(lev, read_u32(lev, 0xb0),
                                        lev.size() - read_u32(lev, 0xb0));
  need(normalized_path(fieldbase + ".fld") == p.field_entry,
       "LEV field does not match project field");
  if (!edited_terrain.empty()) {
    const auto offset = read_u32(lev, 0xb4);
    auto base = normalized_path(read_utf16le_z(lev, offset, lev.size() - offset));
    const auto dot = base.find_last_of('.');
    const auto slash = base.find_last_of('/');
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash)) base.resize(dot);
    for (auto &[slot, gfx] : edited_terrain) {
      auto path = base + static_cast<char>('a' + slot) + ".gfx";
      p.assets.insert_or_assign(std::move(path), std::move(gfx));
    }
  }
  const auto placements = read_u32(lev, 0xd8), offset = read_u32(lev, 0xdc);
  need(offset <= lev.size() &&
           uint64_t(offset) + uint64_t(placements) * 32 <= lev.size(),
       "Invalid LEV placement array");
  p.total_factions = read_u32(lev, 0x314);
  p.human_factions = read_u32(lev, 0x310);
  need(p.total_factions >= 1 && p.total_factions <= 7,
       "Map needs 1..7 active factions");
  for (uint32_t f = 1; f <= p.total_factions; ++f) {
    const uint32_t family = f + read_u32(lev, 0x21c + (f - 1) * 0x20);
    p.human_allowed[f] = family >= 1 && family <= 7;
  }
  p.default_human_faction = std::clamp(p.human_factions, 1U, p.total_factions);
  if (!p.human_allowed[p.default_human_faction]) {
    p.default_human_faction = 0;
    for (uint32_t f = 1; f <= p.total_factions; ++f)
      if (p.human_allowed[f]) {
        p.default_human_faction = f;
        break;
      }
  }
  if (auto it = meta.find("default_human_faction"); it != meta.end()) {
    uint32_t f = 0;
    auto parsed = std::from_chars(it->second.data(),
                                  it->second.data() + it->second.size(), f);
    need(parsed.ec == std::errc{} &&
             parsed.ptr == it->second.data() + it->second.size(),
         "Invalid default human faction");
    p.default_human_faction = f;
  }
  need(p.default_human_faction >= 1 &&
           p.default_human_faction <= p.total_factions &&
           p.human_allowed[p.default_human_faction],
       "Default human faction is not playable");
  // Custom games expose all active factions for selection; their original
  // human-prefix count is not a player limit.
  write_u32(lev, 0x310, p.total_factions);
  need(fld.size() >= 512 && read_u32(fld, 0) == fourcc("fld") &&
           read_u32(fld, 4) == fld.size(),
       "Invalid project FLD");
  p.width = read_u32(fld, 0xb8);
  p.height = read_u32(fld, 0xbc);
  need(p.width >= 4 && p.height >= 4 && p.height <= 257 &&
           uint64_t(p.width) * p.height <= 65536 &&
           512ULL + 128ULL * p.width * p.height == fld.size(),
       "Unsupported terrain dimensions");
  auto converter = read_u32(fld, 12);
  need(converter == 0x60006 || converter == 0x70001,
       "Unsupported FLD converter");
  write_u32(fld, 12, 0x60006);
  if (!p.string_entry.empty()) {
    const auto strings = TextResourceAsset::parse(p.assets.at(p.string_entry));
    need(strings.string_count() >= 16, "Project needs 16 level text entries");
    for (unsigned i = 0; i < 16; ++i)
      p.texts.push_back(strings.string(i));
  } else
    p.texts.resize(16); // Original V1 projects did not yet carry a STR entry.
  p.title = meta.contains("display_name") ? meta.at("display_name")
                                          : file.stem().string();
  p.author = meta.contains("author") && !meta.at("author").empty()
                 ? meta.at("author")
                 : "Unknown";
  // Use a private alias; the runtime text provider supplies it without
  // replacing stock alias pages.
  write_u32(lev, 0x170, 0x10000);
  return p;
}
std::vector<std::filesystem::path>
discover(const std::filesystem::path &directory) {
  std::vector<std::filesystem::path> result;
  std::error_code ec;
  if (!std::filesystem::is_directory(directory, ec))
    return result;
  for (std::filesystem::directory_iterator i(directory, ec), end;
       i != end && !ec; i.increment(ec)) {
    if (i->is_regular_file(ec)) {
      auto ext = i->path().extension().string();
      for (auto &c : ext)
        if (c >= 'A' && c <= 'Z')
          c += 32;
      if (ext == ".thmap")
        result.push_back(i->path());
    }
  }
  std::sort(result.begin(), result.end());
  return result;
}
} // namespace custom_maps
