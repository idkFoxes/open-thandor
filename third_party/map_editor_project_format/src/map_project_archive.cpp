#include "thandor/map_project_archive.hpp"

#include "thandor/gfx.hpp"
#include "miniz.h"

#include <algorithm>
#include <charconv>
#include <memory>
#include <set>
#include <sstream>

namespace thandor {
namespace {
// open-thandor: the game's limits (the editor allows 64 / 32 / 128 MiB)
constexpr uint64_t kMaxArchiveSize = kMapProjectMaxArchiveBytes;
constexpr uint64_t kMaxEntrySize = 16ULL * 1024 * 1024;
constexpr uint64_t kMaxDecodedSize = 64ULL * 1024 * 1024;
constexpr std::size_t kMaxEntries = 4096;
constexpr std::size_t kMaxManifestSize = 512 * 1024;

void check_entry_size(uint64_t size, uint64_t& total) {
    if (size > kMaxEntrySize || total + size > kMaxDecodedSize)
        throw FormatError("map project exceeds the decoded asset size limit");
    total += size;
}

struct ZipReader {
    mz_zip_archive handle{};
    explicit ZipReader(std::span<const std::byte> data) {
        if (!mz_zip_reader_init_mem(&handle, data.data(), data.size(), 0))
            throw FormatError("invalid map project ZIP");
    }
    ~ZipReader() { mz_zip_reader_end(&handle); }
};

struct ZipWriter {
    mz_zip_archive handle{};
    ZipWriter() {
        if (!mz_zip_writer_init_heap(&handle, 0, 0))
            throw FormatError("could not initialize map project ZIP");
    }
    ~ZipWriter() { mz_zip_writer_end(&handle); }
};

std::string_view trim(std::string_view value) {
    constexpr std::string_view whitespace = " \t\r";
    const auto start = value.find_first_not_of(whitespace);
    if (start == std::string_view::npos) return {};
    return value.substr(start, value.find_last_not_of(whitespace) - start + 1);
}

int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

std::string unescape_value(std::string_view value) {
    std::string result;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const int high = hex_digit(value[i + 1]), low = hex_digit(value[i + 2]);
            if (high >= 0 && low >= 0) {
                if (high == 0 && low == 0) throw FormatError("NUL in project manifest value");
                result.push_back(static_cast<char>(high * 16 + low));
                i += 2;
                continue;
            }
        }
        result.push_back(value[i]);
    }
    return result;
}
} // namespace

std::string normalize_project_path(std::string_view path) {
    std::string name(path);
    for (auto& c : name) {
        if (c == '\\') c = '/';
        else if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    if (name.empty() || name.size() > 1024 || name.find(':') != std::string::npos ||
        std::ranges::any_of(name, [](unsigned char c) { return c < 32; }))
        throw FormatError("invalid map project asset path");
    std::string_view rest(name);
    for (;;) {
        const auto slash = rest.find('/');
        const auto part = rest.substr(0, slash);
        if (part.empty() || part == "." || part == "..")
            throw FormatError("invalid map project path component");
        if (slash == std::string_view::npos) break;
        rest.remove_prefix(slash + 1);
    }
    return name;
}

MapProjectArchive MapProjectArchive::open(const std::filesystem::path& path) {
    if (std::filesystem::file_size(path) > kMaxArchiveSize)
        throw FormatError("map project exceeds 32 MiB");
    return parse(read_file(path));
}

MapProjectArchive MapProjectArchive::parse(std::vector<std::byte> bytes) {
    if (bytes.size() > kMaxArchiveSize)
        throw FormatError("map project exceeds 32 MiB");

    MapProjectArchive result;
    result.zip = bytes.size() >= 4 &&
        (read_u32(bytes, 0) == 0x04034b50u || read_u32(bytes, 0) == 0x06054b50u);
    uint64_t total = 0;
    if (result.zip) {
        ZipReader reader(bytes);
        auto& zip = reader.handle;
        const auto count = mz_zip_reader_get_num_files(&zip);
        if (count > kMaxEntries) throw FormatError("too many project ZIP entries");
        for (mz_uint i = 0; i < count; ++i) {
            mz_zip_archive_file_stat stat{};
            const auto length = mz_zip_reader_get_filename(&zip, i, nullptr, 0);
            if (length < 2 || length > 1025 || !mz_zip_reader_file_stat(&zip, i, &stat))
                throw FormatError("invalid project ZIP entry");
            std::string name(length, '\0');
            if (mz_zip_reader_get_filename(&zip, i, name.data(), length) != length)
                throw FormatError("invalid project ZIP filename");
            name.pop_back();
            if (stat.m_is_directory && (name.ends_with('/') || name.ends_with('\\'))) name.pop_back();
            name = normalize_project_path(name);
            if (stat.m_is_directory) continue;
            if (!stat.m_is_supported || stat.m_is_encrypted)
                throw FormatError("unsupported compression or encryption in project ZIP");
            check_entry_size(stat.m_uncomp_size, total);
            auto [entry, inserted] = result.files.try_emplace(std::move(name));
            if (!inserted) throw FormatError("duplicate project ZIP path");
            auto& data = entry->second;
            data.resize(static_cast<std::size_t>(stat.m_uncomp_size));
            if (!mz_zip_reader_extract_to_mem(&zip, i, data.data(), data.size(), 0))
                throw FormatError("invalid or damaged project ZIP entry: " + entry->first);
        }
    } else {
        const auto pck = PckArchive::parse("map project", std::move(bytes));
        if (pck.entries().size() > kMaxEntries) throw FormatError("too many legacy project entries");
        for (const auto& entry : pck.entries()) {
            check_entry_size(entry.unpacked_size, total);
            const auto name = normalize_project_path(entry.name);
            if (result.files.contains(name)) throw FormatError("duplicate legacy project path");
            result.files.emplace(name, pck.extract(entry));
        }
    }
    return result;
}

const std::vector<std::byte>& MapProjectArchive::require(std::string_view path) const {
    const auto it = files.find(normalize_project_path(path));
    if (it == files.end()) throw FormatError("missing map project entry: " + std::string(path));
    return it->second;
}

MapProjectManifest MapProjectArchive::manifest() const {
    const auto& bytes = require(zip ? kZipMapManifest : kLegacyMapManifest);
    if (bytes.empty() || bytes.size() > kMaxManifestSize)
        throw FormatError("invalid map project manifest size");
    std::string_view text(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    const auto nul = text.find('\0');
    if (zip && nul != std::string_view::npos) throw FormatError("NUL in map.ini");
    text = text.substr(0, nul);
    if (text.starts_with("\xef\xbb\xbf")) text.remove_prefix(3);

    std::istringstream input{std::string(text)};
    std::string line;
    std::getline(input, line);
    if (zip ? trim(line) != "[map]" :
        (trim(line) != "THANDOR_MAP_EDITOR_PROJECT_V1" &&
         trim(line) != "THANDOR_MAP_EDITOR_PROJECT_V2" &&
         trim(line) != "THANDOR_MAP_EDITOR_PROJECT_V3"))
        throw FormatError("unsupported map project manifest");

    MapProjectManifest result;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto stripped = trim(line);
        if (stripped.empty() || stripped.starts_with(';') || stripped.starts_with('#')) continue;
        const auto separator = stripped.find('=');
        if (separator == std::string_view::npos) {
            if (zip) throw FormatError("invalid map.ini line");
            continue;
        }
        const auto key = trim(stripped.substr(0, separator));
        const auto value = std::string_view(line).substr(line.find('=') + 1);
        if (key.empty() || !result.emplace(key, unescape_value(zip ? trim(value) : value)).second)
            throw FormatError("empty or duplicate map project key");
    }
    if (zip && (!result.contains("format") || result.at("format") != "1"))
        throw FormatError("unsupported map.ini format version");
    return result;
}

std::string editable_terrain_image_path(uint32_t slot) {
    if (slot >= 26) throw FormatError("invalid terrain texture slot");
    return std::string(kEditableTerrainDirectory) + static_cast<char>('a' + slot) + ".tga";
}

uint32_t map_project_crc32(std::span<const std::byte> bytes) {
    return static_cast<uint32_t>(mz_crc32(0, reinterpret_cast<const unsigned char*>(bytes.data()), bytes.size()));
}

std::map<uint32_t, std::vector<std::byte>> read_edited_terrain_images(
    const MapProjectArchive& archive, const MapProjectManifest& manifest) {
    std::map<uint32_t, std::vector<std::byte>> edits;
    if (!archive.zip) return edits;
    for (uint32_t slot = 0; slot < 26; ++slot) {
        const auto path = editable_terrain_image_path(slot);
        const auto file = archive.files.find(path);
        const auto key = manifest.find("terrain_crc." + std::to_string(slot));
        if (file == archive.files.end()) continue;
        if (key != manifest.end()) {
            uint32_t original{};
            const auto* end = key->second.data() + key->second.size();
            const auto parsed = std::from_chars(key->second.data(), end, original);
            if (parsed.ec != std::errc{} || parsed.ptr != end)
                throw FormatError("invalid terrain texture checksum in map.ini");
            if (map_project_crc32(file->second) == original) continue;
        }
        const auto& bytes = file->second;
        if (bytes.size() < 18 || read_u16(bytes, 12) != 256 || read_u16(bytes, 14) != 256)
            throw FormatError("editable ground texture must be 256x256: " + path);
        const auto image = decode_tga_rgba(bytes);
        edits.emplace(slot, encode_single_rgba_gfx(image));
    }
    return edits;
}

void write_zip_map_project(const std::filesystem::path& path, std::span<const PckWriteEntry> entries) {
    if (entries.size() > kMaxEntries) throw FormatError("too many map project files");
    ZipWriter writer;
    auto& zip = writer.handle;
    uint64_t total = 0;
    std::set<std::string> names;
    for (const auto& entry : entries) {
        const auto name = normalize_project_path(entry.name);
        if (!names.insert(name).second) throw FormatError("duplicate project ZIP path: " + name);
        check_entry_size(entry.data.size(), total);
        if (!mz_zip_writer_add_mem(&zip, name.c_str(), entry.data.data(), entry.data.size(), MZ_DEFAULT_LEVEL))
            throw FormatError("could not add map project entry: " + name);
    }
    void* output = nullptr;
    std::size_t size = 0;
    if (!mz_zip_writer_finalize_heap_archive(&zip, &output, &size))
        throw FormatError("could not finalize map project ZIP");
    const std::unique_ptr<void, decltype(&mz_free)> data(output, &mz_free);
    if (size > kMaxArchiveSize) throw FormatError("map project ZIP exceeds 32 MiB");
    write_file_atomic(std::filesystem::absolute(path), {static_cast<const std::byte*>(data.get()), size});
}

} // namespace thandor
