#include "thandor/text_resource.hpp"

#include <algorithm>
#include <array>
#include <limits>

namespace thandor {
namespace {
constexpr std::size_t kHeaderSize = 0x200;
constexpr std::size_t kLocaleCountOffset = 0xB0;
constexpr std::size_t kBlockPrefixSize = 0x10;

void pad_four(std::vector<std::byte>& bytes) {
    while ((bytes.size() & 3U) != 0U) bytes.push_back(std::byte{0});
}
}

TextResourceAsset TextResourceAsset::parse(std::span<const std::byte> data) {
    if (data.size() < kHeaderSize || read_u32(data, 0) != fourcc("str")) {
        throw FormatError("invalid STR text-resource magic");
    }
    if (read_u32(data, 4) != data.size()) throw FormatError("STR allocation size mismatch");
    TextResourceAsset asset;
    asset.header_.assign(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(kHeaderSize));
    const auto locale_count = read_u32(data, kLocaleCountOffset);
    // Every locale occupies at least a 0x10-byte prefix. Validate before
    // allocating a vector from an untrusted STR header.
    if(locale_count > (data.size()-kHeaderSize)/kBlockPrefixSize)
        throw FormatError("STR locale count exceeds asset capacity");
    std::size_t cursor = kHeaderSize;
    asset.blocks_.reserve(locale_count);
    for (uint32_t locale = 0; locale < locale_count; ++locale) {
        if (cursor > data.size() || kBlockPrefixSize > data.size() - cursor) {
            throw FormatError("STR locale block prefix exceeds asset bounds");
        }
        const auto block_size = read_u32(data, cursor + 0x00);
        const auto string_count = read_u32(data, cursor + 0x04);
        if (block_size < kBlockPrefixSize + static_cast<uint64_t>(string_count) * 4ULL ||
            cursor + block_size > data.size() || (block_size & 3U) != 0U) {
            throw FormatError("STR locale block has invalid size");
        }
        LocaleBlock block;
        block.country_code = read_u32(data, cursor + 0x08);
        block.reserved = read_u32(data, cursor + 0x0C);
        block.strings.reserve(string_count);
        std::vector<uint32_t> offsets;
        offsets.reserve(string_count + 1U);
        for (uint32_t index = 0; index < string_count; ++index) {
            const auto offset = read_u32(data, cursor + kBlockPrefixSize + index * 4U);
            const auto minimum = kBlockPrefixSize + string_count * 4U;
            if (offset < minimum || offset >= block_size || (offset & 1U) != 0U) {
                throw FormatError("STR string offset is outside its locale block");
            }
            if (!offsets.empty() && offset < offsets.back()) throw FormatError("STR string offsets are not ordered");
            offsets.push_back(offset);
        }
        offsets.push_back(block_size);
        for (uint32_t index = 0; index < string_count; ++index) {
            const auto begin = cursor + offsets[index];
            const auto end = cursor + offsets[index + 1U];
            block.strings.emplace_back(data.begin() + static_cast<std::ptrdiff_t>(begin),
                                       data.begin() + static_cast<std::ptrdiff_t>(end));
        }
        asset.blocks_.push_back(std::move(block));
        cursor += block_size;
    }
    if (cursor != data.size()) throw FormatError("STR locale-block walk did not end at allocation boundary");
    return asset;
}

std::vector<std::byte> TextResourceAsset::encode_plain_utf16(std::string_view text) {
    std::vector<std::byte> temporary((text.size() + 1U) * 4U + 4U);
    write_utf16le_fixed(temporary, 0, temporary.size(), text);
    std::size_t end = 0;
    while (end + 1U < temporary.size()) {
        if (read_u16(temporary, end) == 0U) {
            end += 2U;
            temporary.resize(end);
            pad_four(temporary);
            return temporary;
        }
        end += 2U;
    }
    throw FormatError("failed to terminate generated UTF-16 text resource string");
}

TextResourceAsset TextResourceAsset::minimal_level_compatibility(std::string_view display_name) {
    TextResourceAsset asset;
    asset.header_.assign(kHeaderSize, std::byte{0});
    write_u32(asset.header_, 0x00, fourcc("str"));
    write_u32(asset.header_, 0x08, 1U);
    write_u32(asset.header_, 0x0C, 0U);
    write_u32(asset.header_, kLocaleCountOffset, 1U);
    LocaleBlock block;
    block.country_code = 44U;
    block.strings.reserve(16U);
    block.strings.push_back(encode_plain_utf16(" " + std::string(display_name)));
    block.strings.push_back(encode_plain_utf16(display_name));
    for (uint32_t index = 2; index < 9; ++index) block.strings.push_back(encode_plain_utf16("Scenario"));
    for (uint32_t index = 9; index < 16; ++index) {
        block.strings.push_back(encode_plain_utf16("Custom map objective"));
    }
    asset.blocks_.push_back(std::move(block));
    return asset;
}

const TextResourceAsset::LocaleBlock& TextResourceAsset::select_block(uint32_t preferred_country) const {
    if (blocks_.empty()) throw FormatError("STR asset has no locale blocks");
    for (const auto& candidate : blocks_) {
        if (candidate.country_code == preferred_country) return candidate;
    }
    for (const auto& candidate : blocks_) {
        if (candidate.country_code == 44U) return candidate;
    }
    return blocks_.front();
}

std::string TextResourceAsset::string(uint32_t index, uint32_t preferred_country) const {
    const auto& block = select_block(preferred_country);
    if (index >= block.strings.size()) throw FormatError("STR string index exceeds selected locale block");
    const auto& bytes = block.strings[index];
    return read_utf16le_z(bytes, 0, bytes.size());
}

uint32_t TextResourceAsset::string_count(uint32_t preferred_country) const {
    const auto count = select_block(preferred_country).strings.size();
    if (count > std::numeric_limits<uint32_t>::max()) throw FormatError("STR string count exceeds 32-bit field");
    return static_cast<uint32_t>(count);
}

std::vector<uint32_t> TextResourceAsset::country_codes() const {
    std::vector<uint32_t> result;
    result.reserve(blocks_.size());
    for (const auto& block : blocks_) result.push_back(block.country_code);
    return result;
}

void TextResourceAsset::ensure_string_count_all_locales(uint32_t count) {
    const auto empty = encode_plain_utf16("");
    for (auto& block : blocks_) {
        while (block.strings.size() < count) block.strings.push_back(empty);
    }
}

void TextResourceAsset::copy_string_all_locales(uint32_t target_index,
                                                 const TextResourceAsset& source,
                                                 uint32_t source_index) {
    ensure_string_count_all_locales(target_index + 1U);
    for (auto& target : blocks_) {
        const auto& selected = source.select_block(target.country_code);
        if (source_index >= selected.strings.size()) {
            throw FormatError("source STR compatibility string is missing");
        }
        target.strings[target_index] = selected.strings[source_index];
    }
}

void TextResourceAsset::replace_plain_string_all_locales(uint32_t index, std::string_view text) {
    ensure_string_count_all_locales(index + 1U);
    const auto replacement = encode_plain_utf16(text);
    for (auto& block : blocks_) block.strings[index] = replacement;
}

std::string TextResourceAsset::compatibility_display_name(uint32_t preferred_country) const {
    auto value = string(0, preferred_country);
    if (!value.empty() && value.front() == ' ') value.erase(value.begin());
    return value;
}

bool TextResourceAsset::has_level_list_mutation_prefix(uint32_t preferred_country) const {
    const auto value = string(0, preferred_country);
    return !value.empty() && value.front() == ' ';
}

void TextResourceAsset::replace_string_zero_all_locales(std::string_view display_name) {
    // The frontend writes 0x8001 over word zero before sorting/drawing the level list. Keep the
    // stock leading-space scratch word so the first visible character is never destroyed.
    const auto replacement = encode_plain_utf16(" " + std::string(display_name));
    for (auto& block : blocks_) {
        if (block.strings.empty()) throw FormatError("STR locale block lacks compatibility string 0");
        block.strings[0] = replacement;
    }
}

std::vector<std::byte> TextResourceAsset::serialize() const {
    if (header_.size() != kHeaderSize || blocks_.empty()) throw FormatError("STR asset is incomplete");
    std::vector<std::byte> output = header_;
    write_u32(output, 0x00, fourcc("str"));
    write_u32(output, kLocaleCountOffset, static_cast<uint32_t>(blocks_.size()));
    for (const auto& block : blocks_) {
        if (block.strings.empty()) throw FormatError("STR locale block has no strings");
        const uint64_t table_end64 = kBlockPrefixSize + static_cast<uint64_t>(block.strings.size()) * 4ULL;
        if (table_end64 > std::numeric_limits<uint32_t>::max()) throw FormatError("STR offset table is too large");
        const auto block_start = output.size();
        output.resize(block_start + static_cast<std::size_t>(table_end64), std::byte{0});
        write_u32(output, block_start + 0x04, static_cast<uint32_t>(block.strings.size()));
        write_u32(output, block_start + 0x08, block.country_code);
        write_u32(output, block_start + 0x0C, block.reserved);
        for (std::size_t index = 0; index < block.strings.size(); ++index) {
            const auto relative = output.size() - block_start;
            if (relative > std::numeric_limits<uint32_t>::max()) throw FormatError("STR string offset exceeds 32-bit field");
            write_u32(output, block_start + kBlockPrefixSize + index * 4U, static_cast<uint32_t>(relative));
            output.insert(output.end(), block.strings[index].begin(), block.strings[index].end());
            pad_four(output);
        }
        const auto block_size = output.size() - block_start;
        if (block_size > std::numeric_limits<uint32_t>::max()) throw FormatError("STR locale block exceeds 32-bit size");
        write_u32(output, block_start + 0x00, static_cast<uint32_t>(block_size));
    }
    if (output.size() > std::numeric_limits<uint32_t>::max()) throw FormatError("STR asset exceeds 32-bit allocation size");
    write_u32(output, 0x04, static_cast<uint32_t>(output.size()));
    return output;
}

} // namespace thandor
