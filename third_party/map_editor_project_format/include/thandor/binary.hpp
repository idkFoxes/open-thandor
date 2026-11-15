#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace thandor {

class FormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

std::vector<std::byte> read_file(const std::filesystem::path& path);
void write_file_atomic(const std::filesystem::path& path, std::span<const std::byte> data);

uint16_t read_u16(std::span<const std::byte> data, std::size_t offset);
uint32_t read_u32(std::span<const std::byte> data, std::size_t offset);
int32_t read_i32(std::span<const std::byte> data, std::size_t offset);
void write_u16(std::span<std::byte> data, std::size_t offset, uint16_t value);
void write_u32(std::span<std::byte> data, std::size_t offset, uint32_t value);
void write_i32(std::span<std::byte> data, std::size_t offset, int32_t value);

std::string read_utf16le_z(std::span<const std::byte> data, std::size_t offset,
                           std::size_t maximum_bytes);
void write_utf16le_fixed(std::span<std::byte> data, std::size_t offset,
                         std::size_t capacity_bytes, std::string_view utf8);
std::string normalize_asset_path(std::string_view path);
std::string lower_ascii(std::string value);
uint32_t fourcc(std::string_view text);
std::string fourcc_to_string(uint32_t value);

constexpr float q12_to_float(int32_t value) noexcept {
    return static_cast<float>(value) / 4096.0F;
}
constexpr int32_t float_to_q12(float value) noexcept {
    return static_cast<int32_t>(value * 4096.0F + (value >= 0.0F ? 0.5F : -0.5F));
}

// Retail audio gains use unsigned Q15 where 0x8000 is unity.  Keep this conversion named
// separately from Q12 world units so UI diagnostics cannot silently mix the two domains.
constexpr float q15_gain_to_float(uint32_t value) noexcept {
    return static_cast<float>(value) / 32768.0F;
}

} // namespace thandor
