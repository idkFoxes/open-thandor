#pragma once

#include "thandor/pck.hpp"

#include <map>

namespace thandor {

inline constexpr std::string_view kZipMapManifest = "map.ini";
inline constexpr std::string_view kLegacyMapManifest = "editor/project.txt";
inline constexpr std::string_view kEditableTerrainDirectory = "textures/terrain/";
inline constexpr uint64_t kMapProjectMaxArchiveBytes = 32ULL * 1024 * 1024; // open-thandor: 32 MiB (editor: 64)

using MapProjectFiles = std::map<std::string, std::vector<std::byte>, std::less<>>;
using MapProjectManifest = std::map<std::string, std::string, std::less<>>;

struct MapProjectArchive {
    MapProjectFiles files;
    bool zip{};

    static MapProjectArchive open(const std::filesystem::path& path);
    static MapProjectArchive parse(std::vector<std::byte> bytes); // open-thandor: the file's bytes
    const std::vector<std::byte>& require(std::string_view path) const;
    MapProjectManifest manifest() const;
};

std::string normalize_project_path(std::string_view path);
std::string editable_terrain_image_path(uint32_t slot);
uint32_t map_project_crc32(std::span<const std::byte> bytes);
std::map<uint32_t, std::vector<std::byte>> read_edited_terrain_images(
    const MapProjectArchive& archive, const MapProjectManifest& manifest);
void write_zip_map_project(const std::filesystem::path& path, std::span<const PckWriteEntry> entries);

} // namespace thandor
