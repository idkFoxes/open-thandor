#pragma once

#include "thandor/binary.hpp"
#include "thandor/semantic_types.hpp"

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace thandor {

struct RgbaImage {
    uint32_t width{};
    uint32_t height{};
    std::vector<uint8_t> pixels; // RGBA8
};

// User-editable terrain textures are exchanged as true-colour TGA files.  The
// stock ground GFX family is one 256x256 direct-colour subresource, so the editor can rebuild a
// game-loadable GFX without quantising the imported pixels through a palette.
RgbaImage decode_tga_rgba(std::span<const std::byte> bytes);
std::vector<std::byte> encode_tga_rgba(const RgbaImage& image);
RgbaImage load_tga_rgba(const std::filesystem::path& path);
void save_tga_rgba(const std::filesystem::path& path, const RgbaImage& image);
std::vector<std::byte> encode_single_rgba_gfx(const RgbaImage& image);

struct GfxSubresource {
    uint32_t logical_width{};
    uint32_t logical_height{};
    PaletteBankIndex palette_bank{};
    uint32_t data_offset{};
    int32_t origin_x{};
    int32_t origin_y{};
    uint32_t pixel_width{};
    uint32_t pixel_height{};
};

class GfxAsset {
public:
    static GfxAsset parse(std::span<const std::byte> data);
    std::span<const GfxSubresource> subresources() const noexcept { return subresources_; }
    std::span<const uint32_t> palette_argb() const noexcept { return palette_argb_; }
    RgbaImage decode(GraphicsSubresourceIndex subresource_index,
                     bool zero_alpha_is_opaque = true) const;
    RgbaImage decode_logical(GraphicsSubresourceIndex subresource_index,
                             bool zero_alpha_is_opaque = true) const;
private:
    std::vector<std::byte> bytes_;
    std::vector<GfxSubresource> subresources_;
    std::vector<uint32_t> palette_argb_;
};

class PalAsset {
public:
    static PalAsset parse(std::span<const std::byte> data);
    std::span<const uint32_t> colors_argb() const noexcept { return colors_argb_; }
private:
    std::vector<uint32_t> colors_argb_;
};

} // namespace thandor
