#pragma once

#include "thandor/binary.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace thandor {

enum class PckCompressionMethod : uint32_t {
    HuffmanRle = 0,
    Stored = 1,
    FieldGrid = 2,
};

struct PckEntry {
    std::size_t index{};
    std::string name;
    std::string normalized_name;
    uint32_t unpacked_size{};
    uint32_t packed_size{};
    uint32_t type_tag{};
    uint32_t directory_runtime_payload_offset{};
    PckCompressionMethod compression_method{PckCompressionMethod::Stored};
    std::size_t header_offset{};
    std::size_t data_offset{};
};

class PckArchive {
public:
    static PckArchive open(const std::filesystem::path& path);
    static PckArchive parse(std::filesystem::path source, std::vector<std::byte> bytes);

    const std::filesystem::path& path() const noexcept { return path_; }
    const std::vector<PckEntry>& entries() const noexcept { return entries_; }
    std::span<const std::byte> header_bytes() const noexcept;
    const PckEntry* find(std::string_view normalized_path) const;
    // Mirrors Package_FindEntryInMount: no slash normalization or case folding is performed.
    const PckEntry* find_exact(std::string_view archive_path) const noexcept;
    std::vector<std::byte> extract(const PckEntry& entry) const;
    std::vector<std::byte> extract(std::string_view normalized_path) const;

private:
    std::filesystem::path path_;
    std::vector<std::byte> bytes_;
    std::vector<PckEntry> entries_;
    std::unordered_map<std::string, std::size_t> lookup_;
};

struct PckWriteEntry {
    std::string name;
    std::vector<std::byte> data;
    uint32_t type_tag{};
};

// Converts the editor's normalized slash form to the exact lowercase backslash form used by
// Package_FindEntryInMount. Unsafe absolute or parent-traversal paths are rejected.
std::string canonical_pck_entry_path(std::string_view path);

std::vector<std::byte> build_stored_pck(
    std::span<const PckWriteEntry> entries,
    std::span<const std::byte> header_template = {});
void write_stored_pck(
    const std::filesystem::path& output,
    std::span<const PckWriteEntry> entries,
    std::span<const std::byte> header_template = {});

} // namespace thandor
