#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <numbers>

namespace thandor {

// Detached four-byte semantic domains recovered by the V338 export.  These wrappers deliberately
// do not change any serialized record layout; they separate values only at API boundaries.
struct GraphicsSubresourceIndex {
    uint32_t raw{};
    explicit constexpr GraphicsSubresourceIndex(uint32_t value = 0U) noexcept : raw(value) {}
    constexpr uint32_t value() const noexcept { return raw; }
    constexpr std::size_t as_size() const noexcept { return static_cast<std::size_t>(raw); }
    friend constexpr bool operator==(GraphicsSubresourceIndex, GraphicsSubresourceIndex) = default;
};

struct PaletteBankIndex {
    static constexpr int32_t kNone = -1;
    int32_t raw{kNone};
    explicit constexpr PaletteBankIndex(int32_t value = kNone) noexcept : raw(value) {}
    constexpr bool present() const noexcept { return raw >= 0; }
    constexpr int32_t value() const noexcept { return raw; }
    constexpr std::size_t as_size() const noexcept { return static_cast<std::size_t>(raw); }
    friend constexpr bool operator==(PaletteBankIndex, PaletteBankIndex) = default;
};

struct ModelLookupKeyIndex {
    uint32_t raw{};
    explicit constexpr ModelLookupKeyIndex(uint32_t value = 0U) noexcept : raw(value) {}
    constexpr uint32_t value() const noexcept { return raw; }
    friend constexpr bool operator==(ModelLookupKeyIndex, ModelLookupKeyIndex) = default;
};

struct ModelLookupKeyClass {
    uint32_t raw{};
    explicit constexpr ModelLookupKeyClass(uint32_t value = 0U) noexcept : raw(value & 0xFU) {}
    constexpr uint32_t value() const noexcept { return raw; }
    friend constexpr bool operator==(ModelLookupKeyClass, ModelLookupKeyClass) = default;
};

struct AngleTurn32 {
    uint32_t raw{};
    explicit constexpr AngleTurn32(uint32_t value = 0U) noexcept : raw(value) {}
    constexpr uint32_t value() const noexcept { return raw; }
    constexpr uint16_t low_turn16() const noexcept { return static_cast<uint16_t>(raw & 0xFFFFU); }
    friend constexpr bool operator==(AngleTurn32, AngleTurn32) = default;
};

struct GraphicsRenderFlagMask {
    uint32_t raw{};
    explicit constexpr GraphicsRenderFlagMask(uint32_t value = 0U) noexcept : raw(value) {}
    constexpr uint32_t value() const noexcept { return raw; }
};

struct DepthBinMask32 {
    uint32_t raw{};
    explicit constexpr DepthBinMask32(uint32_t value = 0U) noexcept : raw(value) {}
    constexpr uint32_t value() const noexcept { return raw; }
};

// LEV and MDL disk records keep their original storage.  Conversion into the four-byte runtime
// angle domain is explicit so a serialized turn16 field cannot be confused with an arbitrary ID.
constexpr AngleTurn32 runtime_angle_from_serialized_turn16(uint32_t serialized) noexcept {
    return AngleTurn32{serialized & 0xFFFFU};
}
constexpr AngleTurn32 runtime_angle_from_storage(uint32_t stored) noexcept {
    return AngleTurn32{stored};
}
constexpr float angle_turn32_to_radians(AngleTurn32 angle) noexcept {
    return static_cast<float>(angle.low_turn16()) *
           (2.0F * std::numbers::pi_v<float> / 65536.0F);
}

constexpr uint32_t pack_model_lookup_key(ModelLookupKeyIndex index,
                                         ModelLookupKeyClass key_class) noexcept {
    return (index.value() << 4U) | key_class.value();
}

} // namespace thandor
