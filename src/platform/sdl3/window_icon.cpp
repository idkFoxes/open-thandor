/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/window_icon.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: the window and taskbar icon from thandor.ico (see thandor/platform/sdl3/window_icon.h).
   ICO layout (all values little-endian): a 6-byte header (reserved 0, type 1 = icon, image count), then one
   16-byte directory entry per image (width, height, colour count, reserved, planes, bits per pixel, data size,
   data offset), then the image data. An image is either a PNG file (skipped here: SDL3 has no PNG decoder) or a
   DIB: a BITMAPINFOHEADER whose height counts both bitmaps (twice the icon height), the palette (1, 4 and 8 bits
   per pixel), the XOR bitmap (the colours) and the 1-bit AND mask (1 = transparent), both bottom-up with rows
   padded to 4 bytes. 32-bit images carry their own alpha; the mask only counts when all alpha bytes are 0. */

#include <thandor/platform/sdl3/window_icon.h>

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <cstring>
#include <string>

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

namespace thandor::sdl3 {
namespace {

constexpr std::size_t kIcoHeaderBytes = 6;
constexpr std::size_t kIcoEntryBytes = 16;
constexpr uint16_t kIcoTypeIcon = 1;
constexpr std::size_t kBitmapInfoHeaderBytes = 40;
constexpr uint32_t kBitmapCompressionRgb = 0; /* BI_RGB */
constexpr int kMaximumIconSide = 1024;         /* larger images are taken as broken */
constexpr int kNominalIconSide = 32;           /* the window icon at 100% display scale */
constexpr uint8_t kPngSignature[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
constexpr const char *kIconFileName = "thandor.ico";

uint32_t ReadU16(std::span<const std::byte> data, std::size_t offset) noexcept
{
  return static_cast<uint32_t>(data[offset]) | (static_cast<uint32_t>(data[offset + 1]) << 8);
}

uint32_t ReadU32(std::span<const std::byte> data, std::size_t offset) noexcept
{
  return ReadU16(data, offset) | (ReadU16(data, offset + 2) << 16);
}

/* Decodes one DIB image (the bytes of one directory entry); false when it is broken or of a kind not handled. */
bool DecodeDibImage(std::span<const std::byte> dib, IconImage &image)
{
  if (dib.size() < kBitmapInfoHeaderBytes) {
    return false;
  }
  const uint32_t headerBytes = ReadU32(dib, 0);
  const int32_t width = static_cast<int32_t>(ReadU32(dib, 4));
  const int32_t doubledHeight = static_cast<int32_t>(ReadU32(dib, 8));
  const uint32_t bitsPerPixel = ReadU16(dib, 14);
  const uint32_t compression = ReadU32(dib, 16);
  const uint32_t usedColours = ReadU32(dib, 32);
  if ((headerBytes < kBitmapInfoHeaderBytes) || (headerBytes > dib.size()) || (width <= 0) ||
      (width > kMaximumIconSide) || (doubledHeight <= 0) || (doubledHeight > 2 * kMaximumIconSide) ||
      (compression != kBitmapCompressionRgb)) {
    return false;
  }
  if ((bitsPerPixel != 1) && (bitsPerPixel != 4) && (bitsPerPixel != 8) && (bitsPerPixel != 24) &&
      (bitsPerPixel != 32)) {
    return false;
  }
  /* the height counts the XOR bitmap and the AND mask */
  const int height = std::max(doubledHeight / 2, 1);
  std::size_t paletteColours = 0;
  if (bitsPerPixel <= 8) {
    paletteColours = (usedColours != 0) ? usedColours : (std::size_t{1} << bitsPerPixel);
    if (paletteColours > 256) {
      return false;
    }
  }
  const std::size_t paletteOffset = headerBytes;
  const std::size_t colourOffset = paletteOffset + paletteColours * 4;
  const std::size_t colourStride = ((static_cast<std::size_t>(width) * bitsPerPixel + 31) / 32) * 4;
  const std::size_t maskOffset = colourOffset + colourStride * static_cast<std::size_t>(height);
  const std::size_t maskStride = ((static_cast<std::size_t>(width) + 31) / 32) * 4;
  if (maskOffset > dib.size()) {
    return false;
  }
  /* a missing mask counts as opaque */
  const bool hasMask = maskOffset + maskStride * static_cast<std::size_t>(height) <= dib.size();

  image.width = width;
  image.height = height;
  image.bitsPerPixel = static_cast<int>(bitsPerPixel);
  image.pixels.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0);
  bool anyAlpha = false;
  for (int y = 0; y < height; y++) {
    /* bottom-up */
    const std::size_t row = colourOffset + colourStride * static_cast<std::size_t>(height - 1 - y);
    for (int x = 0; x < width; x++) {
      uint32_t blue = 0;
      uint32_t green = 0;
      uint32_t red = 0;
      uint32_t alpha = 0xFF;
      if (bitsPerPixel >= 24) {
        const std::size_t pixel = row + static_cast<std::size_t>(x) * (bitsPerPixel / 8);
        blue = static_cast<uint32_t>(dib[pixel]);
        green = static_cast<uint32_t>(dib[pixel + 1]);
        red = static_cast<uint32_t>(dib[pixel + 2]);
        if (bitsPerPixel == 32) {
          alpha = static_cast<uint32_t>(dib[pixel + 3]);
          anyAlpha = anyAlpha || (alpha != 0);
        }
      }
      else {
        const std::size_t bitIndex = static_cast<std::size_t>(x) * bitsPerPixel;
        const uint32_t packed = static_cast<uint32_t>(dib[row + bitIndex / 8]);
        const uint32_t shift = 8 - bitsPerPixel - static_cast<uint32_t>(bitIndex % 8);
        const std::size_t colourIndex = (packed >> shift) & ((1u << bitsPerPixel) - 1);
        if (colourIndex < paletteColours) {
          const std::size_t entry = paletteOffset + colourIndex * 4;
          blue = static_cast<uint32_t>(dib[entry]);
          green = static_cast<uint32_t>(dib[entry + 1]);
          red = static_cast<uint32_t>(dib[entry + 2]);
        }
      }
      image.pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] =
           (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
  }
  /* the AND mask: always for 1..24 bits per pixel, for 32 bits only when the image has no alpha */
  if ((bitsPerPixel != 32) || !anyAlpha) {
    for (int y = 0; y < height; y++) {
      const std::size_t row = maskOffset + maskStride * static_cast<std::size_t>(height - 1 - y);
      for (int x = 0; x < width; x++) {
        bool transparent = false;
        if (hasMask) {
          transparent = ((static_cast<uint32_t>(dib[row + static_cast<std::size_t>(x) / 8]) >> (7 - x % 8)) & 1) != 0;
        }
        uint32_t &pixel =
             image.pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
        pixel = (pixel & 0x00FFFFFFu) | (transparent ? 0u : 0xFF000000u);
      }
    }
  }
  return true;
}

/* An image is preferred over another of the same size when it has more bits per pixel. */
bool SameSize(const IconImage &a, const IconImage &b) noexcept
{
  return (a.width == b.width) && (a.height == b.height);
}

/* The surface of an image (ARGB8888), nullptr when SDL cannot make one. */
SDL_Surface *SurfaceOfImage(const IconImage &image) noexcept
{
  SDL_Surface *surface = SDL_CreateSurface(image.width, image.height, SDL_PIXELFORMAT_ARGB8888);
  if (surface == nullptr) {
    return nullptr;
  }
  for (int y = 0; y < image.height; y++) {
    std::memcpy(static_cast<uint8_t *>(surface->pixels) + static_cast<std::size_t>(y) * surface->pitch,
                image.pixels.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width),
                static_cast<std::size_t>(image.width) * sizeof(uint32_t));
  }
  return surface;
}

/* Reads a whole file with SDL; empty when it cannot be read. */
std::vector<std::byte> LoadWholeFile(const std::string &path)
{
  std::vector<std::byte> bytes;
  std::size_t size = 0;
  void *data = SDL_LoadFile(path.c_str(), &size);
  if (data != nullptr) {
    bytes.assign(static_cast<const std::byte *>(data), static_cast<const std::byte *>(data) + size);
    SDL_free(data);
  }
  return bytes;
}

} // namespace

bool DecodeIcoFile(std::span<const std::byte> data, IconFile &out)
{
  out = IconFile{};
  if ((data.size() < kIcoHeaderBytes) || (ReadU16(data, 0) != 0) || (ReadU16(data, 2) != kIcoTypeIcon)) {
    return false;
  }
  const std::size_t entryCount = ReadU16(data, 4);
  if ((entryCount == 0) || (kIcoHeaderBytes + entryCount * kIcoEntryBytes > data.size())) {
    return false;
  }
  for (std::size_t entryIndex = 0; entryIndex < entryCount; entryIndex++) {
    const std::size_t entry = kIcoHeaderBytes + entryIndex * kIcoEntryBytes;
    const std::size_t imageBytes = ReadU32(data, entry + 8);
    const std::size_t imageOffset = ReadU32(data, entry + 12);
    if ((imageOffset > data.size()) || (imageBytes > data.size() - imageOffset)) {
      out.skippedInvalidImages++;
      continue;
    }
    const std::span<const std::byte> imageData = data.subspan(imageOffset, imageBytes);
    if ((imageData.size() >= sizeof kPngSignature) &&
        (std::memcmp(imageData.data(), kPngSignature, sizeof kPngSignature) == 0)) {
      out.skippedPngImages++;
      continue;
    }
    IconImage image;
    if (DecodeDibImage(imageData, image)) {
      out.images.push_back(std::move(image));
    }
    else {
      out.skippedInvalidImages++;
    }
  }
  return true;
}

std::size_t IconPrimaryImageIndex(const IconFile &icon)
{
  std::size_t best = icon.images.size();
  for (std::size_t index = 0; index < icon.images.size(); index++) {
    if (best == icon.images.size()) {
      best = index;
      continue;
    }
    const IconImage &candidate = icon.images[index];
    const IconImage &current = icon.images[best];
    if (SameSize(candidate, current)) {
      if (candidate.bitsPerPixel > current.bitsPerPixel) {
        best = index;
      }
      continue;
    }
    /* rank: the nominal size, then the smallest larger one, then the largest smaller one */
    const int candidateSide = std::max(candidate.width, candidate.height);
    const int currentSide = std::max(current.width, current.height);
    const bool candidateLarge = candidateSide >= kNominalIconSide;
    const bool currentLarge = currentSide >= kNominalIconSide;
    if (candidateLarge != currentLarge) {
      if (candidateLarge) {
        best = index;
      }
    }
    else if (candidateLarge ? (candidateSide < currentSide) : (candidateSide > currentSide)) {
      best = index;
    }
  }
  return best;
}

std::vector<std::size_t> IconAlternateImageIndices(const IconFile &icon, std::size_t primaryIndex)
{
  std::vector<std::size_t> alternates;
  for (std::size_t index = 0; index < icon.images.size(); index++) {
    const IconImage &candidate = icon.images[index];
    if ((primaryIndex < icon.images.size()) && SameSize(candidate, icon.images[primaryIndex])) {
      continue;
    }
    auto sameSize = std::find_if(alternates.begin(), alternates.end(), [&](std::size_t chosen) {
      return SameSize(icon.images[chosen], candidate);
    });
    if (sameSize == alternates.end()) {
      alternates.push_back(index);
    }
    else if (candidate.bitsPerPixel > icon.images[*sameSize].bitsPerPixel) {
      *sameSize = index;
    }
  }
  std::sort(alternates.begin(), alternates.end(), [&](std::size_t a, std::size_t b) {
    return icon.images[a].width * icon.images[a].height < icon.images[b].width * icon.images[b].height;
  });
  return alternates;
}

void SetWindowIconFromGameDirectory(SDL_Window *window) noexcept
{
  try {
    /* the executable's directory, else the current directory */
    std::string path;
    std::vector<std::byte> bytes;
    const char *basePath = SDL_GetBasePath();
    if (basePath != nullptr) {
      path = std::string(basePath) + kIconFileName;
      bytes = LoadWholeFile(path);
    }
    if (bytes.empty()) {
      path = kIconFileName;
      bytes = LoadWholeFile(path);
    }
    if (bytes.empty()) {
      Thandor_Log("window icon: no %s in the game directory, default icon", kIconFileName);
      return;
    }
    IconFile icon;
    if (!DecodeIcoFile(bytes, icon)) {
      Thandor_Log("window icon: %s is no icon file, default icon", path.c_str());
      return;
    }
    const std::size_t primaryIndex = IconPrimaryImageIndex(icon);
    if (primaryIndex >= icon.images.size()) {
      Thandor_Log("window icon: %s has no usable image (%d PNG, %d broken), default icon", path.c_str(),
                  icon.skippedPngImages, icon.skippedInvalidImages);
      return;
    }
    const IconImage &primary = icon.images[primaryIndex];
    SDL_Surface *surface = SurfaceOfImage(primary);
    if (surface == nullptr) {
      Thandor_Log("window icon: no surface (%s), default icon", SDL_GetError());
      return;
    }
    std::string sizes;
    for (std::size_t alternateIndex : IconAlternateImageIndices(icon, primaryIndex)) {
      const IconImage &image = icon.images[alternateIndex];
      SDL_Surface *alternate = SurfaceOfImage(image);
      if ((alternate != nullptr) && SDL_AddSurfaceAlternateImage(surface, alternate)) {
        sizes += ' ' + std::to_string(image.width) + 'x' + std::to_string(image.height);
      }
      SDL_DestroySurface(alternate); /* the surface keeps its own reference */
    }
    if (SDL_SetWindowIcon(window, surface)) {
      Thandor_Log("window icon: %s, %dx%d %d-bit, alternate sizes:%s%s", path.c_str(), primary.width,
                  primary.height, primary.bitsPerPixel, sizes.empty() ? " none" : sizes.c_str(),
                  (icon.skippedPngImages + icon.skippedInvalidImages != 0) ? " (some images skipped)" : "");
    }
    else {
      Thandor_Log("window icon: SDL_SetWindowIcon failed (%s), default icon", SDL_GetError());
    }
    SDL_DestroySurface(surface);
  }
  catch (...) {
    Thandor_Log("window icon: out of memory, default icon");
  }
}

} // namespace thandor::sdl3
