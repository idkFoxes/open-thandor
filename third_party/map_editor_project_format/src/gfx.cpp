#include "thandor/gfx.hpp"

#include <algorithm>
#include <array>
#include <limits>

namespace thandor {
namespace {
constexpr uint32_t kGfxMagic = 0x00786667U;
constexpr uint32_t kPalMagic = 0x006C6170U;
constexpr std::size_t kHeaderSize = 0x200;
constexpr std::size_t kSourceEntrySize = 0x20;

void write_u32_to(std::span<std::byte> bytes, std::size_t offset, uint32_t value) {
    if (offset > bytes.size() || sizeof(value) > bytes.size() - offset) {
        throw FormatError("write_u32 exceeds output buffer");
    }
    for (std::size_t index = 0; index < 4U; ++index) {
        bytes[offset + index] = static_cast<std::byte>((value >> (index * 8U)) & 0xFFU);
    }
}

void require_rgba_image(const RgbaImage& image, const char* label) {
    if (image.width == 0U || image.height == 0U || image.width > 16384U || image.height > 16384U) {
        throw FormatError(std::string(label) + " has invalid dimensions");
    }
    const uint64_t expected = static_cast<uint64_t>(image.width) * image.height * 4ULL;
    if (expected != image.pixels.size()) {
        throw FormatError(std::string(label) + " RGBA payload size does not match dimensions");
    }
}

void append_argb(std::vector<uint8_t>& out, uint32_t value, bool zero_alpha_is_opaque) {
    uint8_t alpha = static_cast<uint8_t>((value >> 24U) & 0xFFU);
    if (alpha == 0 && zero_alpha_is_opaque) alpha = 0xFF;
    out.push_back(static_cast<uint8_t>((value >> 16U) & 0xFFU));
    out.push_back(static_cast<uint8_t>((value >> 8U) & 0xFFU));
    out.push_back(static_cast<uint8_t>(value & 0xFFU));
    out.push_back(alpha);
}

} // namespace

RgbaImage decode_tga_rgba(std::span<const std::byte> bytes) {
    constexpr std::size_t header_size = 18;
    if (bytes.size() < header_size) throw FormatError("TGA header is truncated");
    const auto byte = [&](std::size_t offset) { return std::to_integer<uint8_t>(bytes[offset]); };
    const uint8_t image_type = byte(2);
    const uint32_t width = read_u16(bytes, 12), height = read_u16(bytes, 14);
    const uint8_t depth = byte(16), descriptor = byte(17);
    if (byte(1) != 0 || (image_type != 2 && image_type != 10) || (depth != 24 && depth != 32))
        throw FormatError("TGA requires a 24-bit or 32-bit true-colour image");
    if (width == 0 || height == 0 || uint64_t(width) * height > 16U * 1024 * 1024)
        throw FormatError("invalid or oversized TGA dimensions");
    if ((descriptor & 0xc0u) != 0) throw FormatError("interleaved TGA is unsupported");

    std::size_t offset = header_size + byte(0);
    if (offset > bytes.size()) throw FormatError("TGA identification field is truncated");
    const std::size_t stride = depth / 8;
    const std::size_t count = std::size_t(width) * height;
    RgbaImage image{width, height, std::vector<uint8_t>(count * 4)};
    const bool top_origin = (descriptor & 0x20u) != 0;
    const bool right_origin = (descriptor & 0x10u) != 0;
    const auto read_pixel = [&]() -> std::array<uint8_t, 4> {
        if (stride > bytes.size() - offset) throw FormatError("TGA pixels are truncated");
        const std::array<uint8_t, 4> rgba{byte(offset + 2), byte(offset + 1), byte(offset),
            stride == 4 ? byte(offset + 3) : uint8_t{255}};
        offset += stride;
        return rgba;
    };
    for (std::size_t pixel = 0; pixel < count;) {
        std::size_t length = count - pixel;
        bool repeated = false;
        if (image_type == 10) {
            if (offset == bytes.size()) throw FormatError("TGA packet is truncated");
            const auto packet = byte(offset++);
            length = (packet & 127u) + 1u;
            repeated = (packet & 128u) != 0;
            if (length > count - pixel) throw FormatError("TGA packet exceeds image dimensions");
        }
        std::array<uint8_t, 4> rgba{};
        for (std::size_t i = 0; i < length; ++i, ++pixel) {
            if (i == 0 || !repeated) rgba = read_pixel();
            auto x = pixel % width, y = pixel / width;
            if (right_origin) x = width - 1 - x;
            if (!top_origin) y = height - 1 - y;
            std::copy(rgba.begin(), rgba.end(), image.pixels.data() + 4 * (y * width + x));
        }
    }
    return image;
}

std::vector<std::byte> encode_tga_rgba(const RgbaImage& image) {
    require_rgba_image(image, "TGA export image");
    std::vector<std::byte> bytes(18 + image.pixels.size());
    bytes[2] = std::byte{2};
    bytes[12] = std::byte(image.width & 255u);
    bytes[13] = std::byte(image.width >> 8);
    bytes[14] = std::byte(image.height & 255u);
    bytes[15] = std::byte(image.height >> 8);
    bytes[16] = std::byte{32};
    bytes[17] = std::byte{0x28}; // top-left origin with eight alpha bits
    for (std::size_t i = 0; i < image.pixels.size(); i += 4) {
        bytes[18 + i] = std::byte(image.pixels[i + 2]);
        bytes[19 + i] = std::byte(image.pixels[i + 1]);
        bytes[20 + i] = std::byte(image.pixels[i]);
        bytes[21 + i] = std::byte(image.pixels[i + 3]);
    }
    return bytes;
}

RgbaImage load_tga_rgba(const std::filesystem::path& path) {
    if (std::filesystem::file_size(path) > 64ULL * 1024 * 1024)
        throw FormatError("TGA texture exceeds 64 MiB");
    return decode_tga_rgba(read_file(path));
}

void save_tga_rgba(const std::filesystem::path& path, const RgbaImage& image) {
    write_file_atomic(std::filesystem::absolute(path), encode_tga_rgba(image));
}

std::vector<std::byte> encode_single_rgba_gfx(const RgbaImage& image) {
    require_rgba_image(image, "GFX source image");
    const uint64_t pixel_bytes = static_cast<uint64_t>(image.width) * image.height * 4ULL;
    const uint64_t total_size64 = kHeaderSize + kSourceEntrySize + pixel_bytes;
    if (total_size64 > std::numeric_limits<uint32_t>::max()) {
        throw FormatError("encoded GFX exceeds the 32-bit allocation-size field");
    }
    const auto total_size = static_cast<std::size_t>(total_size64);
    std::vector<std::byte> bytes(total_size, std::byte{0});
    const uint32_t table_offset = static_cast<uint32_t>(kHeaderSize);
    const uint32_t data_offset = static_cast<uint32_t>(kHeaderSize + kSourceEntrySize);
    write_u32_to(bytes, 0x00U, kGfxMagic);
    write_u32_to(bytes, 0x04U, static_cast<uint32_t>(total_size));
    write_u32_to(bytes, 0x08U, 1U);
    write_u32_to(bytes, 0xB0U, 1U);
    write_u32_to(bytes, 0xB4U, 0U);
    write_u32_to(bytes, 0xB8U, table_offset);
    write_u32_to(bytes, table_offset + 0x00U, image.width);
    write_u32_to(bytes, table_offset + 0x04U, image.height);
    write_u32_to(bytes, table_offset + 0x08U, 0xFFFFFFFFU); // direct ARGB32 source
    write_u32_to(bytes, table_offset + 0x0CU, data_offset);
    write_u32_to(bytes, table_offset + 0x10U, 0U);
    write_u32_to(bytes, table_offset + 0x14U, 0U);
    write_u32_to(bytes, table_offset + 0x18U, image.width);
    write_u32_to(bytes, table_offset + 0x1CU, image.height);
    for (std::size_t pixel = 0; pixel < image.pixels.size() / 4U; ++pixel) {
        const std::size_t source = pixel * 4U;
        const uint32_t argb = static_cast<uint32_t>(image.pixels[source + 3U]) << 24U |
                              static_cast<uint32_t>(image.pixels[source + 0U]) << 16U |
                              static_cast<uint32_t>(image.pixels[source + 1U]) << 8U |
                              static_cast<uint32_t>(image.pixels[source + 2U]);
        write_u32_to(bytes, data_offset + pixel * 4U, argb);
    }
    return bytes;
}

GfxAsset GfxAsset::parse(std::span<const std::byte> data) {
    if (data.size() < kHeaderSize || read_u32(data, 0) != kGfxMagic) throw FormatError("invalid GFX magic");
    const auto allocation_size = read_u32(data, 4);
    if (allocation_size != 0 && allocation_size > data.size()) throw FormatError("GFX allocation size exceeds payload");
    const auto subresource_count = read_u32(data, 0xB0);
    const auto palette_bank_count = read_u32(data, 0xB4);
    const auto table_offset = read_u32(data, 0xB8);
    if (subresource_count == 0 || subresource_count > 4096) throw FormatError("invalid GFX subresource count");
    const uint64_t palette_entries = static_cast<uint64_t>(palette_bank_count) * 256ULL;
    const uint64_t palette_end = kHeaderSize + palette_entries * 8ULL;
    const uint64_t table_end = static_cast<uint64_t>(table_offset) + static_cast<uint64_t>(subresource_count) * kSourceEntrySize;
    if (palette_end > data.size() || table_offset < kHeaderSize || table_end > data.size()) throw FormatError("GFX table exceeds payload");
    GfxAsset result;
    result.bytes_.assign(data.begin(), data.end());
    result.palette_argb_.reserve(static_cast<std::size_t>(palette_entries));
    for (uint64_t i = 0; i < palette_entries; ++i) result.palette_argb_.push_back(read_u32(data, kHeaderSize + static_cast<std::size_t>(i) * 8));
    result.subresources_.reserve(subresource_count);
    for (uint32_t i = 0; i < subresource_count; ++i) {
        const auto offset = static_cast<std::size_t>(table_offset) + i * kSourceEntrySize;
        GfxSubresource source;
        source.logical_width = read_u32(data, offset + 0x00);
        source.logical_height = read_u32(data, offset + 0x04);
        source.palette_bank = PaletteBankIndex{read_i32(data, offset + 0x08)};
        source.data_offset = read_u32(data, offset + 0x0C);
        source.origin_x = read_i32(data, offset + 0x10);
        source.origin_y = read_i32(data, offset + 0x14);
        source.pixel_width = read_u32(data, offset + 0x18);
        source.pixel_height = read_u32(data, offset + 0x1C);
        if (source.pixel_width == 0 || source.pixel_height == 0 || source.pixel_width > 16384 || source.pixel_height > 16384) throw FormatError("invalid GFX dimensions");
        const uint64_t pixels = static_cast<uint64_t>(source.pixel_width) * source.pixel_height;
        const uint64_t bytes = source.palette_bank.present() ? pixels : pixels * 4ULL;
        if (source.data_offset > data.size() || bytes > data.size() - source.data_offset) throw FormatError("GFX pixel payload exceeds bounds");
        if (source.palette_bank.present() && static_cast<uint32_t>(source.palette_bank.value()) >= palette_bank_count) throw FormatError("GFX subresource uses missing palette bank");
        result.subresources_.push_back(source);
    }
    return result;
}

RgbaImage GfxAsset::decode(GraphicsSubresourceIndex subresource_index, bool zero_alpha_is_opaque) const {
    if (subresource_index.as_size() >= subresources_.size()) throw std::out_of_range("GFX subresource index");
    const auto& source = subresources_[subresource_index.as_size()];
    RgbaImage image{source.pixel_width, source.pixel_height, {}};
    const auto count = static_cast<std::size_t>(source.pixel_width) * source.pixel_height;
    image.pixels.reserve(count * 4);
    if (source.palette_bank.present()) {
        const auto palette_base = source.palette_bank.as_size() * 256;
        for (std::size_t i = 0; i < count; ++i) {
            const auto index = std::to_integer<uint8_t>(bytes_[source.data_offset + i]);
            append_argb(image.pixels, palette_argb_[palette_base + index], zero_alpha_is_opaque);
        }
    } else {
        for (std::size_t i = 0; i < count; ++i) append_argb(image.pixels, read_u32(bytes_, source.data_offset + i * 4), zero_alpha_is_opaque);
    }
    return image;
}

RgbaImage GfxAsset::decode_logical(GraphicsSubresourceIndex subresource_index, bool zero_alpha_is_opaque) const {
    if (subresource_index.as_size() >= subresources_.size()) throw std::out_of_range("GFX subresource index");
    const auto& source = subresources_[subresource_index.as_size()];
    if (source.logical_width == 0 || source.logical_height == 0 ||
        source.logical_width > 16384 || source.logical_height > 16384) {
        throw FormatError("invalid GFX logical dimensions");
    }
    const uint64_t pixels64 = static_cast<uint64_t>(source.logical_width) * source.logical_height;
    if (pixels64 > std::numeric_limits<std::size_t>::max() / 4U) throw FormatError("GFX logical canvas is too large");
    RgbaImage canvas{source.logical_width, source.logical_height,
                     std::vector<uint8_t>(static_cast<std::size_t>(pixels64) * 4U, 0)};
    const auto decoded = decode(subresource_index, zero_alpha_is_opaque);
    for (uint32_t y = 0; y < decoded.height; ++y) {
        const int64_t destination_y = static_cast<int64_t>(source.origin_y) + y;
        if (destination_y < 0 || destination_y >= static_cast<int64_t>(canvas.height)) continue;
        for (uint32_t x = 0; x < decoded.width; ++x) {
            const int64_t destination_x = static_cast<int64_t>(source.origin_x) + x;
            if (destination_x < 0 || destination_x >= static_cast<int64_t>(canvas.width)) continue;
            const std::size_t source_offset = (static_cast<std::size_t>(y) * decoded.width + x) * 4U;
            const std::size_t destination_offset =
                (static_cast<std::size_t>(destination_y) * canvas.width + static_cast<std::size_t>(destination_x)) * 4U;
            std::copy_n(decoded.pixels.begin() + static_cast<std::ptrdiff_t>(source_offset), 4,
                        canvas.pixels.begin() + static_cast<std::ptrdiff_t>(destination_offset));
        }
    }
    return canvas;
}

PalAsset PalAsset::parse(std::span<const std::byte> data) {
    if (data.size() < kHeaderSize || read_u32(data, 0) != kPalMagic) throw FormatError("invalid PAL magic");
    const auto allocation_size = read_u32(data, 4);
    if (allocation_size != 0 && allocation_size > data.size()) throw FormatError("PAL allocation size exceeds payload");
    const auto entry_count = read_u32(data, 0xB0);
    const uint64_t end = kHeaderSize + static_cast<uint64_t>(entry_count) * 8ULL;
    if (entry_count == 0 || end > data.size()) throw FormatError("PAL entry count exceeds payload");
    PalAsset result;
    result.colors_argb_.reserve(entry_count);
    for (uint32_t i = 0; i < entry_count; ++i) result.colors_argb_.push_back(read_u32(data, kHeaderSize + i * 8));
    return result;
}

} // namespace thandor
