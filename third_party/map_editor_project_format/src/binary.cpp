#include "thandor/binary.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cctype>
#include <fstream>
#include <limits>

namespace thandor {
namespace {

void require_range(std::span<const std::byte> data, std::size_t offset, std::size_t count) {
    if (offset > data.size() || count > data.size() - offset) {
        throw FormatError("binary field exceeds input bounds");
    }
}

uint32_t decode_utf8_codepoint(std::string_view text, std::size_t& cursor) {
    const auto lead = static_cast<unsigned char>(text[cursor++]);
    if (lead < 0x80) return lead;
    int continuation = 0;
    uint32_t value = 0;
    if ((lead & 0xE0U) == 0xC0U) { continuation = 1; value = lead & 0x1FU; }
    else if ((lead & 0xF0U) == 0xE0U) { continuation = 2; value = lead & 0x0FU; }
    else if ((lead & 0xF8U) == 0xF0U) { continuation = 3; value = lead & 0x07U; }
    else throw FormatError("invalid UTF-8 lead byte");
    if (cursor + static_cast<std::size_t>(continuation) > text.size()) {
        throw FormatError("truncated UTF-8 sequence");
    }
    for (int i = 0; i < continuation; ++i) {
        const auto byte = static_cast<unsigned char>(text[cursor++]);
        if ((byte & 0xC0U) != 0x80U) throw FormatError("invalid UTF-8 continuation byte");
        value = (value << 6U) | (byte & 0x3FU);
    }
    if (value > 0x10FFFFU || (value >= 0xD800U && value <= 0xDFFFU)) {
        throw FormatError("invalid UTF-8 code point");
    }
    return value;
}

void append_utf8(std::string& out, uint32_t cp) {
    if (cp < 0x80U) out.push_back(static_cast<char>(cp));
    else if (cp < 0x800U) {
        out.push_back(static_cast<char>(0xC0U | (cp >> 6U)));
        out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
    } else if (cp < 0x10000U) {
        out.push_back(static_cast<char>(0xE0U | (cp >> 12U)));
        out.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
        out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
    } else {
        out.push_back(static_cast<char>(0xF0U | (cp >> 18U)));
        out.push_back(static_cast<char>(0x80U | ((cp >> 12U) & 0x3FU)));
        out.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
        out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
    }
}

} // namespace

std::vector<std::byte> read_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("cannot open file: " + path.string());
    const auto end = file.tellg();
    if (end < 0) throw std::runtime_error("cannot determine file size: " + path.string());
    const auto size = static_cast<std::size_t>(end);
    std::vector<std::byte> data(size);
    file.seekg(0);
    if (size != 0 && !file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size))) {
        throw std::runtime_error("cannot read file: " + path.string());
    }
    return data;
}

void write_file_atomic(const std::filesystem::path& path, std::span<const std::byte> data) {
    std::filesystem::create_directories(path.parent_path());
    auto temporary = path;
    temporary += ".tmp";
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("cannot create file: " + temporary.string());
        if (!data.empty()) {
            file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        }
        if (!file) throw std::runtime_error("cannot write file: " + temporary.string());
    }
    std::error_code ec;
    std::filesystem::rename(temporary, path, ec);
    if (ec) {
        std::filesystem::remove(path, ec);
        ec.clear();
        std::filesystem::rename(temporary, path, ec);
    }
    if (ec) throw std::runtime_error("cannot replace file: " + path.string() + ": " + ec.message());
}

uint16_t read_u16(std::span<const std::byte> data, std::size_t offset) {
    require_range(data, offset, 2);
    return static_cast<uint16_t>(std::to_integer<uint8_t>(data[offset])) |
           static_cast<uint16_t>(std::to_integer<uint8_t>(data[offset + 1])) << 8U;
}

uint32_t read_u32(std::span<const std::byte> data, std::size_t offset) {
    require_range(data, offset, 4);
    return static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset])) |
           static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 1])) << 8U |
           static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 2])) << 16U |
           static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 3])) << 24U;
}

int32_t read_i32(std::span<const std::byte> data, std::size_t offset) {
    return std::bit_cast<int32_t>(read_u32(data, offset));
}

void write_u16(std::span<std::byte> data, std::size_t offset, uint16_t value) {
    require_range(data, offset, 2);
    data[offset] = static_cast<std::byte>(value & 0xFFU);
    data[offset + 1] = static_cast<std::byte>((value >> 8U) & 0xFFU);
}

void write_u32(std::span<std::byte> data, std::size_t offset, uint32_t value) {
    require_range(data, offset, 4);
    for (int i = 0; i < 4; ++i) data[offset + i] = static_cast<std::byte>((value >> (i * 8U)) & 0xFFU);
}

void write_i32(std::span<std::byte> data, std::size_t offset, int32_t value) {
    write_u32(data, offset, std::bit_cast<uint32_t>(value));
}

std::string read_utf16le_z(std::span<const std::byte> data, std::size_t offset,
                           std::size_t maximum_bytes) {
    require_range(data, offset, maximum_bytes);
    std::string result;
    for (std::size_t pos = 0; pos + 1 < maximum_bytes; pos += 2) {
        uint32_t cp = read_u16(data, offset + pos);
        if (cp == 0) break;
        if (cp >= 0xD800U && cp <= 0xDBFFU) {
            if (pos + 3 >= maximum_bytes) throw FormatError("truncated UTF-16 surrogate pair");
            const auto low = read_u16(data, offset + pos + 2);
            if (low < 0xDC00U || low > 0xDFFFU) throw FormatError("invalid UTF-16 surrogate pair");
            cp = 0x10000U + ((cp - 0xD800U) << 10U) + (low - 0xDC00U);
            pos += 2;
        } else if (cp >= 0xDC00U && cp <= 0xDFFFU) {
            throw FormatError("unexpected UTF-16 low surrogate");
        }
        append_utf8(result, cp);
    }
    return result;
}

void write_utf16le_fixed(std::span<std::byte> data, std::size_t offset,
                         std::size_t capacity_bytes, std::string_view utf8) {
    if ((capacity_bytes & 1U) != 0 || capacity_bytes < 2) throw FormatError("invalid UTF-16 field capacity");
    require_range(data, offset, capacity_bytes);
    std::fill(data.begin() + static_cast<std::ptrdiff_t>(offset),
              data.begin() + static_cast<std::ptrdiff_t>(offset + capacity_bytes), std::byte{0});
    std::vector<uint16_t> words;
    for (std::size_t cursor = 0; cursor < utf8.size();) {
        const auto cp = decode_utf8_codepoint(utf8, cursor);
        if (cp <= 0xFFFFU) words.push_back(static_cast<uint16_t>(cp));
        else {
            const auto value = cp - 0x10000U;
            words.push_back(static_cast<uint16_t>(0xD800U + (value >> 10U)));
            words.push_back(static_cast<uint16_t>(0xDC00U + (value & 0x3FFU)));
        }
    }
    if ((words.size() + 1U) * 2U > capacity_bytes) throw FormatError("UTF-16 value exceeds fixed field");
    for (std::size_t i = 0; i < words.size(); ++i) write_u16(data, offset + i * 2U, words[i]);
}

std::string normalize_asset_path(std::string_view path) {
    std::string out;
    out.reserve(path.size());
    bool last_slash = false;
    for (char ch : path) {
        const char c = ch == '\\' ? '/' : static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (c == '/') {
            if (!last_slash && !out.empty()) out.push_back('/');
            last_slash = true;
        } else {
            out.push_back(c);
            last_slash = false;
        }
    }
    while (!out.empty() && out.front() == '/') out.erase(out.begin());
    while (!out.empty() && out.back() == '/') out.pop_back();
    return out;
}

std::string lower_ascii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

uint32_t fourcc(std::string_view text) {
    uint32_t value = 0;
    for (std::size_t i = 0; i < std::min<std::size_t>(4, text.size()); ++i) {
        value |= static_cast<uint32_t>(static_cast<unsigned char>(text[i])) << (i * 8U);
    }
    return value;
}

std::string fourcc_to_string(uint32_t value) {
    std::string result;
    for (int i = 0; i < 4; ++i) {
        const char c = static_cast<char>((value >> (i * 8U)) & 0xFFU);
        if (c == '\0') break;
        result.push_back(c);
    }
    return result;
}

} // namespace thandor
