#include "thandor/pck.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <queue>
#include <unordered_set>

namespace thandor {
namespace {
constexpr std::size_t kHeaderSize = 0x200;
constexpr std::size_t kEntryHeaderSize = 0x200;
constexpr std::size_t kEntryNameBytes = 0x1EC;

class BitReader {
public:
    explicit BitReader(std::span<const std::byte> data) : data_(data) {}
    void require_dword_window() const {
        const auto token_byte = (cursor_ * 8U - bits_) / 8U;
        if (data_.size() - token_byte < 4U) throw FormatError("PCK token lacks native dword lookahead");
    }
    uint32_t read(unsigned count) {
        while (bits_ < count) {
            if (cursor_ >= data_.size()) throw FormatError("PCK bitstream ended early");
            buffer_ |= static_cast<uint64_t>(std::to_integer<uint8_t>(data_[cursor_++])) << bits_;
            bits_ += 8;
        }
        const uint64_t mask = count == 32 ? 0xFFFFFFFFULL : ((1ULL << count) - 1ULL);
        const auto value = static_cast<uint32_t>(buffer_ & mask);
        buffer_ >>= count;
        bits_ -= count;
        return value;
    }
private:
    std::span<const std::byte> data_;
    std::size_t cursor_{};
    uint64_t buffer_{};
    unsigned bits_{};
};

struct HuffNode { int left{-1}; int right{-1}; };

std::pair<std::array<HuffNode, 512>, int> build_huffman(std::span<const std::byte> frequencies) {
    if (frequencies.size() != 256) throw FormatError("PCK method 0 lacks frequency table");
    std::array<uint32_t, 512> weights{};
    std::array<HuffNode, 512> nodes{};
    for (std::size_t i = 0; i < 256; ++i) weights[i] = std::to_integer<uint8_t>(frequencies[i]);
    int next = 256;
    while (true) {
        int first = -1, second = -1;
        uint32_t first_weight = std::numeric_limits<uint32_t>::max();
        uint32_t second_weight = std::numeric_limits<uint32_t>::max();
        for (int i = 0; i < 512; ++i) {
            const auto weight = weights[static_cast<std::size_t>(i)];
            if (weight == 0) continue;
            if (weight < first_weight) {
                if (first_weight < second_weight) { second_weight = first_weight; second = first; }
                first_weight = weight; first = i;
            } else if (weight < second_weight) {
                second_weight = weight; second = i;
            }
        }
        if (first < 0) throw FormatError("PCK method 0 has no symbols");
        if (second < 0) return {nodes, first};
        if (next >= 512) throw FormatError("PCK Huffman tree exceeds 512 nodes");
        weights[static_cast<std::size_t>(next)] = first_weight + second_weight;
        nodes[static_cast<std::size_t>(next)] = {first, second};
        weights[static_cast<std::size_t>(first)] = 0;
        weights[static_cast<std::size_t>(second)] = 0;
        ++next;
    }
}

std::vector<std::byte> decode_method0(std::span<const std::byte> payload, std::size_t output_size) {
    // Match the native decoder's nonempty output and four-byte lookahead contract.
    if (output_size==0 || payload.size() < 260) throw FormatError("PCK method 0 payload or output is too small");
    const auto [nodes, root] = build_huffman(payload.first(256));
    if(root<256)throw FormatError("PCK method 0 requires at least two symbols");
    BitReader bits(payload.subspan(256));
    std::vector<std::byte> output;
    output.reserve(output_size);
    while (output.size() < output_size) {
        bits.require_dword_window();
        const auto run_length = bits.read(1) ? bits.read(4) + 3U : 1U;
        int node = root;
        while (node >= 256) {
            const auto& branch = nodes[static_cast<std::size_t>(node)];
            node = bits.read(1) ? branch.right : branch.left;
            if (node < 0) throw FormatError("invalid PCK Huffman branch");
        }
        for (uint32_t i = 0; i < run_length && output.size() < output_size; ++i) {
            output.push_back(static_cast<std::byte>(node));
        }
    }
    return output;
}

std::vector<std::byte> decode_method2(std::span<const std::byte> payload, std::size_t output_size) {
    if (payload.size() < 0x10) throw FormatError("PCK method 2 wrapper is too small");
    const auto intermediate_size = read_u32(payload, 0);
    if (intermediate_size < 0x200) throw FormatError("PCK method 2 intermediate is too small");
    auto intermediate = decode_method0(payload.subspan(0x10), intermediate_size);
    const auto width = read_u32(intermediate, 0xB8);
    const auto height = read_u32(intermediate, 0xBC);
    const uint64_t count = static_cast<uint64_t>(width) * height;
    if(count==0)throw FormatError("PCK method 2 field has no cells");
    const uint64_t expected_intermediate = 0x200ULL + count * 0x10ULL;
    const uint64_t expected_output = 0x200ULL + count * 0x80ULL;
    if (intermediate.size() != expected_intermediate || output_size != expected_output) {
        throw FormatError("PCK method 2 dimensions disagree with declared sizes");
    }
    std::vector<std::byte> output(output_size);
    std::copy_n(intermediate.begin(), 0x200, output.begin());
    int32_t row_base_x = 0;
    int32_t world_y = 0;
    std::size_t source = 0x200;
    std::size_t destination = 0x200;
    for (uint32_t y = 0; y < height; ++y) {
        int32_t world_x = row_base_x;
        for (uint32_t x = 0; x < width; ++x) {
            write_u32(output, destination + 0x54, read_u32(intermediate, source + 0x00));
            write_u32(output, destination + 0x48, read_u32(intermediate, source + 0x04));
            write_u32(output, destination + 0x4C, read_u32(intermediate, source + 0x08));
            write_u32(output, destination + 0x50, read_u32(intermediate, source + 0x0C));
            write_i32(output, destination + 0x40, world_x);
            write_i32(output, destination + 0x44, world_y);
            source += 0x10;
            destination += 0x80;
            world_x += 0x901;
        }
        row_base_x += 0x480;
        world_y -= 1999;
    }
    return output;
}

uint32_t infer_type_tag(const PckWriteEntry& entry) {
    if (entry.type_tag != 0) return entry.type_tag;
    // Package_UpsertEntry writes the first source dword, not the filename extension. This matters
    // for scenario DAT records: stock levelNN.dat uses the first two UTF-16 identifier characters
    // as its directory type tag. Extension-derived "dat" was accepted by our parser but did not
    // reproduce the game's own package writer contract.
    if (entry.data.size() >= 4U) return read_u32(entry.data, 0U);
    const auto dot = entry.name.find_last_of('.');
    if (dot == std::string::npos) return 0;
    return fourcc(lower_ascii(entry.name.substr(dot + 1)));
}

void initialize_canonical_header(std::span<std::byte> header) {
    std::fill(header.begin(), header.end(), std::byte{0});
    write_u32(header, 0, fourcc("pck"));
    write_u32(header, 8, 1U);
    write_u32(header, 0x0C, 0x00010000U);
    // Stock packages carry producer/computer labels in both fixed UTF-16 fields. These labels are
    // metadata only, but emitting them keeps new archives structurally stock-compatible.
    write_utf16le_fixed(header, 0x30, 0x40, "THANDOR MAP EDITOR");
    write_utf16le_fixed(header, 0x70, 0x40, "THANDOR MAP EDITOR");
}

} // namespace

PckArchive PckArchive::open(const std::filesystem::path& path) {
    return parse(path, read_file(path));
}

PckArchive PckArchive::parse(std::filesystem::path source, std::vector<std::byte> bytes) {
    if (bytes.size() < kHeaderSize || read_u32(bytes, 0) != fourcc("pck")) {
        throw FormatError("invalid PCK header: " + source.string());
    }
    if (read_u32(bytes, 4) != bytes.size()) throw FormatError("PCK declared size mismatch: " + source.string());
    PckArchive archive;
    archive.path_ = std::move(source);
    archive.bytes_ = std::move(bytes);
    const auto count = read_u32(archive.bytes_, 0xB0);
    // Reject impossible counts before reserve(), which otherwise turns a
    // malformed project directory into an opaque std::bad_alloc.
    if(count > (archive.bytes_.size()-kHeaderSize)/kEntryHeaderSize)
        throw FormatError("PCK directory count exceeds available entry headers");
    std::size_t offset = kHeaderSize;
    archive.entries_.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        if (offset > archive.bytes_.size() || kEntryHeaderSize > archive.bytes_.size() - offset) {
            throw FormatError("PCK entry header exceeds archive bounds");
        }
        PckEntry entry;
        entry.index = i;
        entry.name = read_utf16le_z(archive.bytes_, offset, kEntryNameBytes);
        entry.normalized_name = normalize_asset_path(entry.name);
        entry.header_offset = offset;
        entry.unpacked_size = read_u32(archive.bytes_, offset + kEntryNameBytes + 4);
        entry.type_tag = read_u32(archive.bytes_, offset + kEntryNameBytes + 8);
        entry.packed_size = read_u32(archive.bytes_, offset + kEntryNameBytes + 12);
        entry.directory_runtime_payload_offset = read_u32(archive.bytes_, offset + kEntryNameBytes + 0);
        entry.compression_method = static_cast<PckCompressionMethod>(
            read_u32(archive.bytes_, offset + kEntryNameBytes + 16));
        entry.data_offset = offset + kEntryHeaderSize;
        if (entry.data_offset > archive.bytes_.size() || entry.packed_size > archive.bytes_.size() - entry.data_offset) {
            throw FormatError("PCK entry payload exceeds archive bounds: " + entry.name);
        }
        archive.lookup_.try_emplace(entry.normalized_name, archive.entries_.size());
        archive.entries_.push_back(std::move(entry));
        offset = archive.entries_.back().data_offset + archive.entries_.back().packed_size;
    }
    if (offset != archive.bytes_.size()) throw FormatError("PCK entry walk did not end at archive boundary");
    return archive;
}

std::span<const std::byte> PckArchive::header_bytes() const noexcept {
    if (bytes_.size() < kHeaderSize) return {};
    return std::span<const std::byte>(bytes_).first(kHeaderSize);
}

const PckEntry* PckArchive::find(std::string_view normalized_path) const {
    const auto key = normalize_asset_path(normalized_path);
    const auto it = lookup_.find(key);
    return it == lookup_.end() ? nullptr : &entries_[it->second];
}

const PckEntry* PckArchive::find_exact(std::string_view archive_path) const noexcept {
    const auto found = std::find_if(entries_.begin(), entries_.end(), [&](const PckEntry& entry) {
        return entry.name == archive_path;
    });
    return found == entries_.end() ? nullptr : &*found;
}

std::vector<std::byte> PckArchive::extract(const PckEntry& entry) const {
    if (entry.data_offset > bytes_.size() || entry.packed_size > bytes_.size() - entry.data_offset) {
        throw FormatError("PCK entry no longer matches archive bounds");
    }
    const auto payload = std::span<const std::byte>(bytes_).subspan(entry.data_offset, entry.packed_size);
    switch (entry.compression_method) {
        case PckCompressionMethod::HuffmanRle: return decode_method0(payload, entry.unpacked_size);
        case PckCompressionMethod::Stored:
            if (payload.size() < entry.unpacked_size) throw FormatError("stored PCK entry is truncated");
            return {payload.begin(), payload.begin() + entry.unpacked_size};
        case PckCompressionMethod::FieldGrid: return decode_method2(payload, entry.unpacked_size);
        default: throw FormatError("unsupported PCK compression method " + std::to_string(static_cast<uint32_t>(entry.compression_method)));
    }
}

std::vector<std::byte> PckArchive::extract(std::string_view normalized_path) const {
    const auto* entry = find(normalized_path);
    if (!entry) throw FormatError("PCK entry not found: " + std::string(normalized_path));
    return extract(*entry);
}

std::string canonical_pck_entry_path(std::string_view path) {
    const std::string normalized = normalize_asset_path(path);
    if (normalized.empty()) throw FormatError("PCK entry path is empty");
    if (normalized.size() >= kEntryNameBytes / 2U) {
        throw FormatError("PCK entry path exceeds the game directory field: " + normalized);
    }
    if (normalized.find(':') != std::string::npos) {
        throw FormatError("PCK entry path may not contain a drive or URI prefix: " + normalized);
    }
    std::size_t component_begin = 0;
    while (component_begin <= normalized.size()) {
        const auto slash = normalized.find('/', component_begin);
        const auto component = normalized.substr(
            component_begin, slash == std::string::npos ? std::string::npos : slash - component_begin);
        if (component.empty() || component == "." || component == "..") {
            throw FormatError("PCK entry path contains an unsafe component: " + normalized);
        }
        if (slash == std::string::npos) break;
        component_begin = slash + 1U;
    }
    std::string exact = normalized;
    std::replace(exact.begin(), exact.end(), '/', '\\');
    return exact;
}

std::vector<std::byte> build_stored_pck(
    std::span<const PckWriteEntry> entries,
    std::span<const std::byte> header_template) {
    uint64_t total = kHeaderSize;
    std::vector<std::string> exact_names;
    exact_names.reserve(entries.size());
    std::unordered_set<std::string> normalized_names;
    normalized_names.reserve(entries.size());
    for (const auto& entry : entries) {
        if ((entry.data.size() & 3U) != 0) {
            throw FormatError("stored PCK entry must be four-byte aligned: " + entry.name);
        }
        auto exact = canonical_pck_entry_path(entry.name);
        const auto normalized = normalize_asset_path(exact);
        if (!normalized_names.emplace(normalized).second) {
            throw FormatError("duplicate PCK entry path after normalization: " + normalized);
        }
        exact_names.push_back(std::move(exact));
        total += kEntryHeaderSize + entry.data.size();
    }
    if (total > std::numeric_limits<uint32_t>::max()) {
        throw FormatError("PCK archive exceeds 32-bit size field");
    }
    std::vector<std::byte> output(static_cast<std::size_t>(total));
    if (header_template.size() >= kHeaderSize) {
        std::copy_n(header_template.begin(), kHeaderSize, output.begin());
    } else {
        initialize_canonical_header(std::span<std::byte>(output).first(kHeaderSize));
    }
    write_u32(output, 0, fourcc("pck"));
    write_u32(output, 4, static_cast<uint32_t>(output.size()));
    write_u32(output, 8, 1U);
    write_u32(output, 0x0C, 0x00010000U);
    write_u32(output, 0xB0, static_cast<uint32_t>(entries.size()));
    std::size_t offset = kHeaderSize;
    for (std::size_t index = 0; index < entries.size(); ++index) {
        const auto& entry = entries[index];
        write_utf16le_fixed(output, offset, kEntryNameBytes, exact_names[index]);
        write_u32(output, offset + kEntryNameBytes + 0, 0);
        write_u32(output, offset + kEntryNameBytes + 4, static_cast<uint32_t>(entry.data.size()));
        write_u32(output, offset + kEntryNameBytes + 8, infer_type_tag(entry));
        write_u32(output, offset + kEntryNameBytes + 12, static_cast<uint32_t>(entry.data.size()));
        write_u32(output, offset + kEntryNameBytes + 16,
                  static_cast<uint32_t>(PckCompressionMethod::Stored));
        std::copy(entry.data.begin(), entry.data.end(),
                  output.begin() + static_cast<std::ptrdiff_t>(offset + kEntryHeaderSize));
        offset += kEntryHeaderSize + entry.data.size();
    }
    return output;
}

void write_stored_pck(
    const std::filesystem::path& output,
    std::span<const PckWriteEntry> entries,
    std::span<const std::byte> header_template) {
    const auto bytes = build_stored_pck(entries, header_template);
    write_file_atomic(output, bytes);
}

} // namespace thandor
