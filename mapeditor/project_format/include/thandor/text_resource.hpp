#pragma once

#include "thandor/binary.hpp"

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace thandor {

class TextResourceAsset {
public:
    static TextResourceAsset parse(std::span<const std::byte> data);
    static TextResourceAsset minimal_level_compatibility(std::string_view display_name);

    uint32_t locale_count() const noexcept { return static_cast<uint32_t>(blocks_.size()); }
    std::string string(uint32_t index, uint32_t preferred_country = 44U) const;
    uint32_t string_count(uint32_t preferred_country = 44U) const;
    std::vector<uint32_t> country_codes() const;
    void ensure_string_count_all_locales(uint32_t count);
    void copy_string_all_locales(uint32_t target_index, const TextResourceAsset& source,
                                 uint32_t source_index);
    void replace_plain_string_all_locales(uint32_t index, std::string_view text);
    // Resource 0 is mutated in place by ScenarioCatalog_RebuildLevelRecordListPage: the game
    // overwrites its first UTF-16 word with rich-text color command 0x8001. Stock level STR files
    // reserve that word with a leading space. These helpers preserve/remove that compatibility
    // prefix without exposing it as part of the map display name.
    std::string compatibility_display_name(uint32_t preferred_country = 44U) const;
    bool has_level_list_mutation_prefix(uint32_t preferred_country = 44U) const;
    void replace_string_zero_all_locales(std::string_view display_name);
    std::vector<std::byte> serialize() const;

private:
    struct LocaleBlock {
        uint32_t country_code{};
        uint32_t reserved{};
        std::vector<std::vector<std::byte>> strings;
    };

    std::vector<std::byte> header_;
    std::vector<LocaleBlock> blocks_;

    static std::vector<std::byte> encode_plain_utf16(std::string_view text);
    const LocaleBlock& select_block(uint32_t preferred_country) const;
};

} // namespace thandor
