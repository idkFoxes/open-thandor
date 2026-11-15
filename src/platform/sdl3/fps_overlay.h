/*
 * Open Thandor
 * Tiny present-only FPS overlay shared by the SDL software and GPU presenters.
 */
#ifndef THANDOR_PLATFORM_SDL3_FPS_OVERLAY_H
#define THANDOR_PLATFORM_SDL3_FPS_OVERLAY_H

#include <algorithm>
#include <cstdint>
#include <cstdio>

namespace thandor::sdl3::fps_overlay {

constexpr int kOriginX = 8;
constexpr int kOriginY = 8;
constexpr int kScale = 2;
constexpr int kGlyphWidth = 3;
constexpr int kGlyphHeight = 5;
constexpr int kAdvance = (kGlyphWidth + 1) * kScale;
constexpr int kMaxCharacters = 8; /* "FPS 9999" */
constexpr int kPadding = 2;
constexpr int kBoxX = kOriginX - kPadding;
constexpr int kBoxY = kOriginY - kPadding;
constexpr int kBoxWidth = kMaxCharacters * kAdvance + kPadding * 2;
constexpr int kBoxHeight = kGlyphHeight * kScale + kPadding * 2;
constexpr uint32_t kBackgroundArgb = 0xFF000000u;
constexpr uint32_t kForegroundArgb = 0xFFFFFFFFu;

inline uint16_t GlyphBits(char c) noexcept
{
  switch (c) {
  case 'F': return 0b111100110100100;
  case 'P': return 0b110101110100100;
  case 'S': return 0b111100111001111;
  case '0': return 0b111101101101111;
  case '1': return 0b010110010010111;
  case '2': return 0b111001111100111;
  case '3': return 0b111001111001111;
  case '4': return 0b101101111001001;
  case '5': return 0b111100111001111;
  case '6': return 0b111100111101111;
  case '7': return 0b111001001001001;
  case '8': return 0b111101111101111;
  case '9': return 0b111101111001111;
  default: return 0;
  }
}

/* emit(x0, y0, x1, y1, argb). The background uses a fixed maximum-size box so software save/restore is constant. */
template <typename Emit>
inline void ForEachRect(uint32_t fps, Emit &&emit) noexcept
{
  emit(kBoxX, kBoxY, kBoxX + kBoxWidth, kBoxY + kBoxHeight, kBackgroundArgb);
  char text[16];
  std::snprintf(text, sizeof text, "FPS %u", std::min<uint32_t>(fps, 9999));
  for (int i = 0; text[i] != '\0'; i++) {
    const uint16_t bits = GlyphBits(text[i]);
    if (bits == 0) {
      continue;
    }
    for (int row = 0; row < kGlyphHeight; row++) {
      for (int col = 0; col < kGlyphWidth; col++) {
        const int bit = (kGlyphHeight - 1 - row) * kGlyphWidth + (kGlyphWidth - 1 - col);
        if ((bits & (uint16_t{1} << bit)) == 0) {
          continue;
        }
        const int x0 = kOriginX + i * kAdvance + col * kScale;
        const int y0 = kOriginY + row * kScale;
        emit(x0, y0, x0 + kScale, y0 + kScale, kForegroundArgb);
      }
    }
  }
}

} // namespace thandor::sdl3::fps_overlay

#endif
